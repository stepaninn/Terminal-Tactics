#include "controller/Controller_mt.h"
#include "model/entity/components/HealthComponent_mt.h"
#include "model/entity/components/InventoryComponent_mt.h"
#include "model/entity/entities/items/Item_mt.h"
#include "model/service/VisionService_mt.h"
#include "model/repository/cells/ItemContainer_mt.h"

#include <algorithm>
#include <thread>
#include <utility>
namespace game::mt::controller {
namespace {
std::string g_io_path;
}

Controller::Controller(game::mt::service::World& world,
                       game::mt::service::TurnService& turn,
                       game::mt::service::MovementService& move,
                       game::mt::service::CombatService& combat,
                       game::mt::service::ItemService& items,
                       game::mt::service::InventoryService& inventory,
                       game::mt::service::AIService& ai)
    : world_(world),
      turn_(turn),
      move_(move),
      combat_(combat),
      items_(items),
      inventory_(inventory),
      ai_(ai),
      player_team_(turn.active_team()) {
    game::mt::service::VisionService vision;
    for (auto id : world_.get_team_entities(turn_.active_team())) {
        vision.update_unit_fov(world_, id);
    }
    vision.rebuild_team_visible(world_, turn_.active_team());
}

bool Controller::handle_action(InputAction action) {
    auto* level = world_.get_level();
    if (!level) return action != InputAction::QUIT;

    auto sorted_items = [](std::vector<std::shared_ptr<const game::mt::entity::items::Item>> items) {
        std::ranges::sort(items, [](const auto& a, const auto& b) {
            if (!a || !b) return a != nullptr;
            return a->get_id() < b->get_id();
        });
        return items;
    };

    if (action != InputAction::QUIT && action != InputAction::NONE) {
        quit_requested_ = false;
    }

    switch (action) {
        case InputAction::MOVE_UP:
            move_cursor(0, -1);
            cell_item_index_ = 0;
            break;
        case InputAction::MOVE_DOWN:
            move_cursor(0, 1);
            cell_item_index_ = 0;
            break;
        case InputAction::MOVE_LEFT:
            move_cursor(-1, 0);
            cell_item_index_ = 0;
            break;
        case InputAction::MOVE_RIGHT:
            move_cursor(1, 0);
            cell_item_index_ = 0;
            break;
        case InputAction::CONFIRM:
            if (mode_ == Mode::SELECT) {
                if (select_entity_at_cursor()) {
                    mode_ = Mode::MOVE;
                }
            } else if (mode_ == Mode::MOVE) {
                move_selected_to_cursor();
            } else if (mode_ == Mode::ATTACK) {
                attack_at_cursor();
            }
            break;
        case InputAction::CANCEL:
            if (mode_ != Mode::SELECT) {
                mode_ = Mode::SELECT;
            } else {
                clear_selection();
            }
            break;
        case InputAction::PICK_ITEM: {
            if (selected_ == game::mt::service::TurnService::kNoEntity) break;
            auto ent = level->get_entity(selected_);
            if (!ent) break;
            auto ent_pos = level->get_entity_position(selected_);
            if (!ent_pos || *ent_pos != cursor_) break;
            auto* cell = level->get_cell(cursor_);
            auto* cont = dynamic_cast<game::mt::repo::cells::IItemContainer*>(cell);
            if (!cont) break;
            auto items = sorted_items(cont->get_items());
            if (items.empty()) break;
            if (cell_item_index_ >= items.size()) cell_item_index_ = 0;
            auto item = items[cell_item_index_];
            if (!item) break;
            if (inventory_.pick_item(*level, selected_, item->get_id())) {
                if (cell_item_index_ > 0 && cell_item_index_ >= items.size() - 1) {
                    cell_item_index_--;
                }
            }
            break;
        }
        case InputAction::DROP_ITEM: {
            if (selected_ == game::mt::service::TurnService::kNoEntity) break;
            auto ent = level->get_entity(selected_);
            if (!ent) break;
            auto* inv = ent->get_component<game::mt::entity::components::InventoryComponent>();
            if (!inv) break;
            auto items = sorted_items(inv->get_items());
            if (items.empty()) break;
            if (inv_item_index_ >= items.size()) inv_item_index_ = 0;
            auto item = items[inv_item_index_];
            if (!item) break;
            if (inventory_.drop_item(*level, selected_, item->get_id())) {
                if (inv_item_index_ > 0 && inv_item_index_ >= items.size() - 1) {
                    inv_item_index_--;
                }
            }
            break;
        }
        case InputAction::USE_ITEM: {
            if (selected_ == game::mt::service::TurnService::kNoEntity) break;
            auto ent = level->get_entity(selected_);
            if (!ent) break;
            auto* inv = ent->get_component<game::mt::entity::components::InventoryComponent>();
            if (!inv) break;
            auto items = sorted_items(inv->get_items());
            if (items.empty()) break;
            if (inv_item_index_ >= items.size()) inv_item_index_ = 0;
            auto item = items[inv_item_index_];
            if (!item) break;
            (void)items_.use_item(*level, selected_, selected_, item->get_id());
            break;
        }
        case InputAction::NEXT_CELL_ITEM: {
            auto* cell = level->get_cell(cursor_);
            auto* cont = dynamic_cast<game::mt::repo::cells::IItemContainer*>(cell);
            if (!cont) break;
            auto items = cont->get_items();
            if (items.empty()) break;
            cell_item_index_ = (cell_item_index_ + 1) % items.size();
            break;
        }
        case InputAction::PREV_CELL_ITEM: {
            auto* cell = level->get_cell(cursor_);
            auto* cont = dynamic_cast<game::mt::repo::cells::IItemContainer*>(cell);
            if (!cont) break;
            auto items = cont->get_items();
            if (items.empty()) break;
            cell_item_index_ = (cell_item_index_ + items.size() - 1) % items.size();
            break;
        }
        case InputAction::NEXT_INV_ITEM: {
            if (selected_ == game::mt::service::TurnService::kNoEntity) break;
            auto ent = level->get_entity(selected_);
            if (!ent) break;
            auto* inv = ent->get_component<game::mt::entity::components::InventoryComponent>();
            if (!inv) break;
            auto items = inv->get_items();
            if (items.empty()) break;
            inv_item_index_ = (inv_item_index_ + 1) % items.size();
            break;
        }
        case InputAction::PREV_INV_ITEM: {
            if (selected_ == game::mt::service::TurnService::kNoEntity) break;
            auto ent = level->get_entity(selected_);
            if (!ent) break;
            auto* inv = ent->get_component<game::mt::entity::components::InventoryComponent>();
            if (!inv) break;
            auto items = inv->get_items();
            if (items.empty()) break;
            inv_item_index_ = (inv_item_index_ + items.size() - 1) % items.size();
            break;
        }
        case InputAction::TOGGLE_MODE:
            if (selected_ == game::mt::service::TurnService::kNoEntity) break;
            mode_ = (mode_ == Mode::MOVE) ? Mode::ATTACK : Mode::MOVE;
            break;
        case InputAction::END_TURN:
            clear_selection();
            if (level) {
                auto teams = turn_.teams();
                for (auto team_id : teams) {
                    if (team_id == player_team_) continue;
                    game::mt::service::TurnService::refresh_team(*level, team_id);
                    auto ids = world_.get_team_entities(team_id);
                    if (ids.empty()) continue;

                    unsigned int hw = std::thread::hardware_concurrency();
                    size_t thread_count = hw == 0 ? 1u : static_cast<size_t>(hw);
                    thread_count = std::min(thread_count, ids.size());
                    size_t chunk = (ids.size() + thread_count - 1) / thread_count;

                    std::vector<std::jthread> workers;
                    workers.reserve(thread_count);
                    for (size_t t = 0; t < thread_count; ++t) {
                        size_t start = t * chunk;
                        size_t end = std::min(ids.size(), start + chunk);
                        if (start >= end) break;
                        workers.emplace_back([this, start, end, &ids] {
                            for (size_t i = start; i < end; ++i) {
                                ai_.act_entity(world_, ids[i]);
                            }
                        });
                    }
                }
                game::mt::service::TurnService::refresh_team(*level, player_team_);
                turn_.set_active_team(player_team_);
                (void)turn_.end_entity_turn();
            }
            break;
        case InputAction::RELOAD:
            if (selected_ != game::mt::service::TurnService::kNoEntity) {
                (void)combat_.reload_weapon(*level, selected_);
            }
            break;
        case InputAction::SAVE_GAME: {
            auto path = take_io_path();
            if (!path.empty()) {
                (void)save_game(path);
            }
            break;
        }
        case InputAction::LOAD_GAME: {
            auto path = take_io_path();
            if (!path.empty()) {
                (void)load_game(path);
            }
            break;
        }
        case InputAction::QUIT:
            if (quit_requested_) return false;
            quit_requested_ = true;
            return true;
        case InputAction::NONE:
        default:
            break;
    }

    level = world_.get_level();
    if (!level) return action != InputAction::QUIT;

    auto entities = level->get_entities();
    for (const auto& entity : entities) {
        if (!entity) continue;
        auto* hp = entity->get_component<game::mt::entity::components::HealthComponent>();
        if (hp && hp->get_current_hp() == 0) {
            if (auto* inv = entity->get_component<game::mt::entity::components::InventoryComponent>()) {
                std::vector<game::mt::ItemId> ids;
                auto items = inv->get_items();
                ids.reserve(items.size());
                for (const auto& item : items) {
                    if (item) ids.push_back(item->get_id());
                }
                for (auto id : ids) {
                    (void)inventory_.drop_item(*level, entity->get_id(), id);
                }
            }
            world_.remove_entity(entity.get());
        }
    }

    game::mt::service::VisionService vision;
    for (auto id : world_.get_team_entities(turn_.active_team())) {
        vision.update_unit_fov(world_, id);
    }
    vision.rebuild_team_visible(world_, turn_.active_team());
    return true;
}

std::vector<game::mt::Position> Controller::get_move_path() const {
    auto* level = world_.get_level();
    if (!level) return {};
    if (mode_ != Mode::MOVE) return {};
    if (selected_ == game::mt::service::TurnService::kNoEntity) return {};
    return move_.find_path(*level, selected_, cursor_);
}

bool Controller::save_game(const std::string& path) const {
    return load_save_.save(world_, path);
}

bool Controller::load_game(const std::string& path) {
    auto loaded = load_save_.load(path);
    if (!loaded) return false;
    auto level = loaded->take_level();
    if (!level) return false;
    world_.set_level(std::move(level));

    clear_selection();
    cursor_ = {0, 0};
    player_team_ = turn_.active_team();

    game::mt::service::VisionService vision;
    for (auto id : world_.get_team_entities(turn_.active_team())) {
        vision.update_unit_fov(world_, id);
    }
    vision.rebuild_team_visible(world_, turn_.active_team());
    return true;
}

void Controller::move_cursor(int dx, int dy) {
    auto* level = world_.get_level();
    if (!level) return;
    game::mt::Position next{cursor_.x + dx, cursor_.y + dy};
    if (!level->in_bounds(next)) return;
    cursor_ = next;
}

bool Controller::select_entity_at_cursor() {
    auto* level = world_.get_level();
    if (!level) return false;
    auto entity = level->get_entity_at(cursor_);
    if (!entity) return false;
    if (entity->get_team_id() != turn_.active_team()) return false;
    if (!turn_.select_entity(*level, entity->get_id())) return false;
    selected_ = entity->get_id();
    inv_item_index_ = 0;
    return true;
}

bool Controller::move_selected_to_cursor() {
    auto* level = world_.get_level();
    if (!level) return false;
    if (selected_ == game::mt::service::TurnService::kNoEntity) return false;
    return move_.move(*level, selected_, cursor_);
}

bool Controller::attack_at_cursor() {
    auto* level = world_.get_level();
    if (!level) return false;
    if (selected_ == game::mt::service::TurnService::kNoEntity) return false;
    return combat_.try_shoot(*level, selected_, cursor_);
}

void Controller::clear_selection() noexcept {
    selected_ = game::mt::service::TurnService::kNoEntity;
    mode_ = Mode::SELECT;
    inv_item_index_ = 0;
    cell_item_index_ = 0;
}

void set_io_path(std::string path) {
    g_io_path = std::move(path);
}

std::string take_io_path() {
    return std::exchange(g_io_path, {});
}

}
