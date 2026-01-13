#include "controller/Controller.h"
#include "model/entity/components/HealthComponent.h"

namespace game::controller {

Controller::Controller(game::service::World& world,
                       game::service::TurnService& turn,
                       game::service::MovementService& move,
                       game::service::CombatService& combat,
                       game::service::ItemService& items)
    : world_(world),
      turn_(turn),
      move_(move),
      combat_(combat),
      items_(items) {}

bool Controller::handle_action(InputAction action) {
    auto* level = world_.get_level();
    if (!level) return action != InputAction::QUIT;

    switch (action) {
        case InputAction::MOVE_UP:
            move_cursor(0, -1);
            break;
        case InputAction::MOVE_DOWN:
            move_cursor(0, 1);
            break;
        case InputAction::MOVE_LEFT:
            move_cursor(-1, 0);
            break;
        case InputAction::MOVE_RIGHT:
            move_cursor(1, 0);
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
        case InputAction::TOGGLE_MODE:
            if (selected_ == game::service::TurnService::kNoEntity) break;
            mode_ = (mode_ == Mode::MOVE) ? Mode::ATTACK : Mode::MOVE;
            break;
        case InputAction::END_TURN:
            clear_selection();
            if (!turn_.end_entity_turn()) {
                (void)turn_.next_team(*level);
            }
            break;
        case InputAction::RELOAD:
            if (selected_ != game::service::TurnService::kNoEntity) {
                (void)combat_.reload_weapon(*level, selected_);
            }
            break;
        case InputAction::QUIT:
            return false;
        case InputAction::NONE:
        default:
            break;
    }

    auto entities = level->get_entities();
    for (auto* entity : entities) {
        if (!entity) continue;
        auto* hp = entity->get_component<game::entity::components::HealthComponent>();
        if (hp && hp->get_current_hp() == 0) {
            world_.remove_entity(entity);
        }
    }
    return true;
}

std::vector<game::Position> Controller::get_move_path() const {
    auto* level = world_.get_level();
    if (!level) return {};
    if (mode_ != Mode::MOVE) return {};
    if (selected_ == game::service::TurnService::kNoEntity) return {};
    return move_.find_path(*level, selected_, cursor_);
}

void Controller::move_cursor(int dx, int dy) {
    auto* level = world_.get_level();
    if (!level) return;
    game::Position next{cursor_.x + dx, cursor_.y + dy};
    if (!level->in_bounds(next)) return;
    cursor_ = next;
}

bool Controller::select_entity_at_cursor() {
    auto* level = world_.get_level();
    if (!level) return false;
    auto* entity = level->get_entity_at(cursor_);
    if (!entity) return false;
    if (entity->get_team_id() != turn_.active_team()) return false;
    if (!turn_.select_entity(*level, entity->get_id())) return false;
    selected_ = entity->get_id();
    return true;
}

bool Controller::move_selected_to_cursor() {
    auto* level = world_.get_level();
    if (!level) return false;
    if (selected_ == game::service::TurnService::kNoEntity) return false;
    return move_.move(*level, selected_, cursor_);
}

bool Controller::attack_at_cursor() {
    auto* level = world_.get_level();
    if (!level) return false;
    if (selected_ == game::service::TurnService::kNoEntity) return false;
    return combat_.try_shoot(*level, selected_, cursor_);
}

void Controller::clear_selection() noexcept {
    selected_ = game::service::TurnService::kNoEntity;
    mode_ = Mode::SELECT;
}

}
