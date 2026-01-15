#include "view/NcursesView.h"

#include "model/entity/components/HealthComponent.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/components/MoveComponent.h"
#include "model/entity/components/TimePointsComponent.h"
#include "model/entity/components/WeaponComponent.h"
#include "model/entity/entities/items/Item.h"
#include "model/entity/entities/items/Weapon.h"
#include "model/repository/Level.h"
#include "model/repository/cells/Floor.h"
#include "model/repository/cells/Glass.h"
#include "model/repository/cells/ItemContainer.h"
#include "model/repository/cells/Partition.h"
#include "model/repository/cells/Stash.h"
#include "model/repository/cells/Wall.h"
#include "model/service/VisionService.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>

namespace game::view {

namespace {
constexpr short kCursorPair = 1;
constexpr short kPathPair = 2;
constexpr short kExploredPair = 3;
constexpr short kFogPair = 4;
constexpr short kFriendlyPair = 5;
constexpr short kEnemyPair = 6;
constexpr short kPathBlockedPair = 7;
constexpr short kGrayColor = 8;

void highlight_cell(int y, int x, short pair, int attr) {
    chtype current = mvinch(y, x);
    chtype base = current & (A_CHARTEXT | A_ALTCHARSET);
    mvaddch(y, x, base | attr | COLOR_PAIR(pair));
}
}

NcursesView::NcursesView(std::shared_ptr<game::service::events::EventBus> bus)
    : bus_(std::move(bus)) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    curs_set(0);
    set_escdelay(25);
    start_color();
    use_default_colors();
    init_pair(kCursorPair, COLOR_BLACK, COLOR_YELLOW);
    init_pair(kPathPair, COLOR_BLACK, COLOR_GREEN);
    init_pair(kPathBlockedPair, COLOR_BLACK, COLOR_RED);
    init_pair(kFriendlyPair, COLOR_GREEN, -1);
    init_pair(kEnemyPair, COLOR_RED, -1);
    if (can_change_color() && COLORS > kGrayColor) {
        init_color(kGrayColor, 500, 500, 500);
        init_pair(kExploredPair, kGrayColor, -1);
        init_pair(kFogPair, kGrayColor, -1);
    } else {
        init_pair(kExploredPair, COLOR_WHITE, -1);
        init_pair(kFogPair, COLOR_WHITE, -1);
    }

    if (bus_) {
        bus_->subscribe<game::service::events::ShotMissedEvent>(
            [this](const std::shared_ptr<game::service::events::ShotMissedEvent>&) {
                last_message_ = "Shot missed";
            });
        bus_->subscribe<game::service::events::ShotMissedAtCellEvent>(
            [this](const std::shared_ptr<game::service::events::ShotMissedAtCellEvent>&) {
                last_message_ = "Shot missed (cell)";
            });
        bus_->subscribe<game::service::events::DamageEvent>(
            [this](const std::shared_ptr<game::service::events::DamageEvent>& ev) {
                last_message_ = "Hit for " + std::to_string(ev->amount);
            });
        bus_->subscribe<game::service::events::WallBrokenEvent>(
            [this](const std::shared_ptr<game::service::events::WallBrokenEvent>&) {
                last_message_ = "Object destroyed";
            });
        bus_->subscribe<game::service::events::ItemUsedEvent>(
            [this](const std::shared_ptr<game::service::events::ItemUsedEvent>&) {
                last_message_ = "Reloaded";
            });
        bus_->subscribe<game::service::events::MoveEvent>(
            [this](const std::shared_ptr<game::service::events::MoveEvent>& ev) {
                if (!ev || !ev->path || ev->path->empty()) return;
                anim_path_ = *ev->path;
                moving_entity_ = static_cast<game::EntityId>(ev->entity_id);
                animating_ = true;
                last_message_ = "Moving";
            });
    }
}

NcursesView::~NcursesView() {
    endwin();
}

game::controller::InputAction NcursesView::poll_input() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
        case 'w':
        case 'W':
            return game::controller::InputAction::MOVE_UP;
        case KEY_DOWN:
        case 's':
        case 'S':
            return game::controller::InputAction::MOVE_DOWN;
        case KEY_LEFT:
        case 'a':
        case 'A':
            return game::controller::InputAction::MOVE_LEFT;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            return game::controller::InputAction::MOVE_RIGHT;
        case '\n':
        case KEY_ENTER:
            return game::controller::InputAction::CONFIRM;
        case 27:
        case KEY_BACKSPACE:
            return game::controller::InputAction::CANCEL;
        case '\t':
        case 'm':
        case 'M':
            return game::controller::InputAction::TOGGLE_MODE;
        case 'e':
        case 'E':
            return game::controller::InputAction::END_TURN;
        case 'r':
        case 'R':
            return game::controller::InputAction::RELOAD;
        case 'p':
        case 'P': {
            auto path = enter_path("Save to: ");
            if (path.empty()) return game::controller::InputAction::NONE;
            game::controller::set_io_path(std::move(path));
            return game::controller::InputAction::SAVE_GAME;
        }
        case 'l':
        case 'L': {
            auto path = enter_path("Load from: ");
            if (path.empty()) return game::controller::InputAction::NONE;
            game::controller::set_io_path(std::move(path));
            return game::controller::InputAction::LOAD_GAME;
        }
        case 'g':
        case 'G':
            return game::controller::InputAction::PICK_ITEM;
        case 't':
        case 'T':
            return game::controller::InputAction::DROP_ITEM;
        case 'u':
        case 'U':
            return game::controller::InputAction::USE_ITEM;
        case '[':
            return game::controller::InputAction::PREV_CELL_ITEM;
        case ']':
            return game::controller::InputAction::NEXT_CELL_ITEM;
        case ',':
            return game::controller::InputAction::PREV_INV_ITEM;
        case '.':
            return game::controller::InputAction::NEXT_INV_ITEM;
        case 'q':
        case 'Q':
            return game::controller::InputAction::QUIT;
        default:
            return game::controller::InputAction::NONE;
    }
}

void NcursesView::render(const game::service::World& world,
                         const game::controller::Controller& controller) {
    erase();
    if (animating_ && !anim_path_.empty()) {
        for (const auto& pos : anim_path_) {
            anim_pos_ = pos;
            draw_map(world, controller);
            draw_hud(world, controller);
            refresh();
            napms(60);
        }
        anim_pos_.reset();
        anim_path_.clear();
        animating_ = false;
    }
    draw_map(world, controller);
    draw_hud(world, controller);
    refresh();
}

static chtype cell_char(const game::repo::cells::ICell* cell) {
    if (!cell) return ' ';
    if (dynamic_cast<const game::repo::cells::Wall*>(cell)) return ACS_BLOCK;
    if (dynamic_cast<const game::repo::cells::Partition*>(cell)) return '+';
    if (dynamic_cast<const game::repo::cells::Glass*>(cell)) return 'g';
    if (dynamic_cast<const game::repo::cells::Stash*>(cell)) return 'S';
    if (dynamic_cast<const game::repo::cells::Floor*>(cell)) return '.';
    return '?';
}

static std::string item_label(const game::entity::items::Item* item) {
    if (!item) return "none";
    if (auto* weapon = dynamic_cast<const game::entity::items::Weapon*>(item)) {
        return "Weapon ammo=" + std::to_string(weapon->get_current_ammo())
            + "/" + std::to_string(weapon->get_max_ammo());
    }
    if (auto* bag = dynamic_cast<const game::entity::items::AmmoBag*>(item)) {
        return "AmmoBag ammo=" + std::to_string(bag->get_current_ammo())
            + "/" + std::to_string(bag->get_max_ammo());
    }
    if (auto* medkit = dynamic_cast<const game::entity::items::Medkit*>(item)) {
        return "Medkit heal=" + std::to_string(medkit->get_heal());
    }
    return "Item";
}

struct Layout {
    int map_x;
    int map_y;
    int map_w;
    int map_h;
    int sel_x;
    int sel_y;
    int sel_w;
    int sel_h;
    int atk_x;
    int atk_y;
    int atk_w;
    int atk_h;
};

static Layout compute_layout(int maxy, int maxx) {
    int side_w = std::clamp(maxx / 3, 24, 40);
    if (maxx - side_w < 20) side_w = std::max(18, maxx - 20);
    int map_w = std::max(10, maxx - side_w);
    int map_h = std::max(6, maxy - 2);
    int right_x = map_w;
    int top_h = std::max(6, map_h / 2);
    int bottom_h = std::max(6, map_h - top_h);
    return Layout{
        0, 0, map_w, map_h,
        right_x, 0, maxx - right_x, top_h,
        right_x, top_h, maxx - right_x, bottom_h
    };
}

static void draw_box(int y, int x, int h, int w, const char* title) {
    if (h <= 1 || w <= 1) return;
    mvhline(y, x, 0, w);
    mvhline(y + h - 1, x, 0, w);
    mvvline(y, x, 0, h);
    mvvline(y, x + w - 1, 0, h);
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + w - 1, ACS_URCORNER);
    mvaddch(y + h - 1, x, ACS_LLCORNER);
    mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER);
    if (title && w > 4) {
        mvprintw(y, x + 2, "%s", title);
    }
}

std::string NcursesView::enter_path(const char* label) {
    int maxy = 0;
    int maxx = 0;
    getmaxyx(stdscr, maxy, maxx);
    int y = std::max(0, maxy - 1);
    move(y, 0);
    clrtoeol();
    mvprintw(y, 0, "%s", label);
    refresh();

    echo();
    curs_set(1);
    char buf[256]{};
    getnstr(buf, static_cast<int>(sizeof(buf) - 1));
    noecho();
    curs_set(0);
    move(y, 0);
    clrtoeol();
    refresh();
    return std::string(buf);
}

static double compute_hit_chance(const game::entity::Entity& attacker,
                                 int distance) {
    auto* combat = attacker.get_component<game::entity::components::CombatComponent>();
    auto* weapon_cmp = attacker.get_component<game::entity::components::WeaponComponent>();
    if (!combat || !weapon_cmp) return 0.0;
    auto* weapon = weapon_cmp->get_weapon();
    if (!weapon) return 0.0;

    int range = weapon->get_range();
    if (range <= 0) return 0.0;
    if (distance == 0) return 1.0;

    double d = std::abs(distance - 1) / static_cast<double>(range);
    d = std::clamp(d, 0.0, 1.0);
    double coef = (1.0 - d * d);
    double capped = std::min(0.95, combat->get_base_accuracy());
    return std::clamp(capped * coef, 0.05, 1.0);
}

void NcursesView::draw_map(const game::service::World& world,
                           const game::controller::Controller& controller) {
    auto* level = world.get_level();
    if (!level) return;

    int maxy = 0;
    int maxx = 0;
    getmaxyx(stdscr, maxy, maxx);

    auto layout = compute_layout(maxy, maxx);
    draw_box(layout.map_y, layout.map_x, layout.map_h, layout.map_w, "MAP");

    int width = static_cast<int>(level->get_width());
    int height = static_cast<int>(level->get_height());
    int inner_w = std::max(1, layout.map_w - 2);
    int inner_h = std::max(1, layout.map_h - 2);
    int draw_w = std::min(width, inner_w);
    int draw_h = std::min(height, inner_h);
    int offset_x = (inner_w - draw_w) / 2;
    int offset_y = (inner_h - draw_h) / 2;

    const auto* team_vis = world.team_visibility(controller.active_team());
    const auto* team_explored = world.team_exploration(controller.active_team());

    int center_x = controller.get_cursor().x;
    int center_y = controller.get_cursor().y;
    if (animating_ && anim_pos_) {
        center_x = anim_pos_->x;
        center_y = anim_pos_->y;
    }
    int cam_x = std::clamp(center_x - draw_w / 2, 0, std::max(0, width - draw_w));
    int cam_y = std::clamp(center_y - draw_h / 2, 0, std::max(0, height - draw_h));

    auto entity_color = [](const game::entity::Entity& ent) {
        return (ent.get_team_id() == 0) ? kFriendlyPair : kEnemyPair;
    };

    for (int y = 0; y < draw_h; ++y) {
        for (int x = 0; x < draw_w; ++x) {
            game::Position pos{cam_x + x, cam_y + y};
            bool visible = team_vis && team_vis->is_visible(pos);
            bool explored = team_explored && team_explored->is_explored(pos);

            int sy = layout.map_y + 1 + offset_y + y;
            int sx = layout.map_x + 1 + offset_x + x;

            const auto* cell = level->get_cell(pos);
            chtype ch = cell_char(cell);
            if (!explored) {
                mvaddch(sy, sx, ' ');
                continue;
            }

            if (visible) {
                if (auto* ent = level->get_entity_at(pos)) {
                    if (!(animating_ && ent->get_id() == moving_entity_)) {
                        ch = (ent->get_id() == controller.selected_entity()) ? '@' : 'O';
                        mvaddch(sy, sx, ch | COLOR_PAIR(entity_color(*ent)));
                        continue;
                    }
                }
                if (auto* cont = dynamic_cast<const game::repo::cells::IItemContainer*>(cell)) {
                    if (cont->size() > 0) {
                        ch = '*';
                    }
                }
                mvaddch(sy, sx, ch);
            } else {
                mvaddch(sy, sx, ch | A_BOLD | A_DIM | COLOR_PAIR(kExploredPair));
            }
        }
    }

    if (animating_ && anim_pos_) {
        if (anim_pos_->x >= cam_x && anim_pos_->y >= cam_y
            && anim_pos_->x < cam_x + draw_w
            && anim_pos_->y < cam_y + draw_h) {
            int sy = layout.map_y + 1 + offset_y + (anim_pos_->y - cam_y);
            int sx = layout.map_x + 1 + offset_x + (anim_pos_->x - cam_x);
            chtype ch = (moving_entity_ == controller.selected_entity()) ? '@' : 'O';
            if (auto* ent = level->get_entity(moving_entity_)) {
                mvaddch(sy, sx, ch | COLOR_PAIR(entity_color(*ent)));
            } else {
                mvaddch(sy, sx, ch);
            }
        }
    }

    auto path = controller.get_move_path();
    int available_steps = 0;
    if (controller.selected_entity() != game::service::TurnService::kNoEntity) {
        if (auto* ent = level->get_entity(controller.selected_entity())) {
            auto* tp = ent->get_component<game::entity::components::TimePointsComponent>();
            auto* mv = ent->get_component<game::entity::components::MoveComponent>();
            if (tp && mv && mv->get_step_cost() > 0) {
                available_steps = tp->get_current_points() / mv->get_step_cost();
            }
        }
    }
    for (size_t i = 0; i < path.size(); ++i) {
        const auto& pos = path[i];
        if (pos.x < cam_x || pos.y < cam_y || pos.x >= cam_x + draw_w || pos.y >= cam_y + draw_h) continue;
        if (team_vis && !team_vis->is_visible(pos)) continue;
        int py = layout.map_y + 1 + offset_y + (pos.y - cam_y);
        int px = layout.map_x + 1 + offset_x + (pos.x - cam_x);
        short pair = (static_cast<int>(i) < available_steps) ? kPathPair : kPathBlockedPair;
        highlight_cell(py, px, pair, A_DIM);
    }

    auto cur = controller.get_cursor();
    bool cursor_blocked = false;
    const auto* cursor_cell = level->get_cell(cur);
    if (!cursor_cell || !cursor_cell->is_walkable()) {
        cursor_blocked = true;
    }

    const game::entity::Entity* selected_ent = nullptr;
    if (controller.selected_entity() != game::service::TurnService::kNoEntity) {
        selected_ent = level->get_entity(controller.selected_entity());
    }
    if (selected_ent) {
        if (auto* target_ent = level->get_entity_at(cur)) {
            if (target_ent->get_id() != selected_ent->get_id()) {
                cursor_blocked = true;
            }
        }
        if (!cursor_blocked && controller.get_mode() == game::controller::Controller::Mode::MOVE) {
            auto* tp = selected_ent->get_component<game::entity::components::TimePointsComponent>();
            auto* mv = selected_ent->get_component<game::entity::components::MoveComponent>();
            if (!tp || !mv || mv->get_step_cost() <= 0) {
                cursor_blocked = true;
            } else {
                auto* sel_pos = level->get_entity_position(selected_ent->get_id());
                if (!sel_pos || !(*sel_pos == cur)) {
                    if (path.empty()) {
                        cursor_blocked = true;
                    } else {
                        int cost = static_cast<int>(path.size()) * mv->get_step_cost();
                        if (tp->get_current_points() < cost) cursor_blocked = true;
                    }
                }
            }
        }
    }
    if (cur.x >= cam_x && cur.y >= cam_y
        && cur.x < cam_x + draw_w
        && cur.y < cam_y + draw_h) {
        int cy = layout.map_y + 1 + offset_y + (cur.y - cam_y);
        int cx = layout.map_x + 1 + offset_x + (cur.x - cam_x);
        short pair = cursor_blocked ? kPathBlockedPair : kCursorPair;
        highlight_cell(cy, cx, pair, A_BOLD);
    }
}

void NcursesView::draw_hud(const game::service::World& world,
                           const game::controller::Controller& controller) {
    int maxy = getmaxy(stdscr);
    int maxx = getmaxx(stdscr);
    auto layout = compute_layout(maxy, maxx);

    draw_box(layout.sel_y, layout.sel_x, layout.sel_h, layout.sel_w, "SELECTED");
    draw_box(layout.atk_y, layout.atk_x, layout.atk_h, layout.atk_w, "ATTACK / LOG");

    const char* mode = "SELECT";
    if (controller.get_mode() == game::controller::Controller::Mode::MOVE) mode = "MOVE";
    if (controller.get_mode() == game::controller::Controller::Mode::ATTACK) mode = "ATTACK";

    auto* level = world.get_level();
    if (!level) return;

    int sel_line = layout.sel_y + 1;
    if (controller.selected_entity() != game::service::TurnService::kNoEntity) {
        auto* ent = level->get_entity(controller.selected_entity());
        if (ent) {
            mvprintw(sel_line++, layout.sel_x + 1, "Selected: %s (team=%llu)",
                     ent->get_name().c_str(),
                     static_cast<unsigned long long>(ent->get_team_id()));
            if (auto* hp = ent->get_component<game::entity::components::HealthComponent>()) {
                mvprintw(sel_line++, layout.sel_x + 1, "HP: %d/%d",
                         hp->get_current_hp(), hp->get_max_hp());
            }
            if (auto* tp = ent->get_component<game::entity::components::TimePointsComponent>()) {
                mvprintw(sel_line++, layout.sel_x + 1, "TP: %d/%d",
                         tp->get_current_points(), tp->get_max_points());
            }
            if (auto* wp = ent->get_component<game::entity::components::WeaponComponent>()) {
                if (auto* weapon = wp->get_weapon()) {
                    mvprintw(sel_line++, layout.sel_x + 1, "Weapon: %d/%d range %d cost %d",
                             weapon->get_current_ammo(), weapon->get_max_ammo(),
                             weapon->get_range(), weapon->get_attack_cost());
                }
            }
            if (auto* inv = ent->get_component<game::entity::components::InventoryComponent>()) {
                mvprintw(sel_line++, layout.sel_x + 1, "Inv: %zu/%d weight %d/%d",
                         inv->size(), inv->get_capacity(),
                         inv->get_weight(), inv->get_max_weight());
                auto items = inv->get_items();
                if (!items.empty()) {
                    mvprintw(sel_line++, layout.sel_x + 1, "Inv item: %s", item_label(items.front()).c_str());
                }
            }
        }
    } else {
        mvprintw(sel_line++, layout.sel_x + 1, "Selected: none");
    }

    int atk_line = layout.atk_y + 1;
    mvprintw(atk_line++, layout.atk_x + 1, "Mode: %s", mode);
    mvprintw(atk_line++, layout.atk_x + 1, "Cursor: (%d,%d)",
             controller.get_cursor().x, controller.get_cursor().y);
    mvprintw(atk_line++, layout.atk_x + 1, "Last: %s", last_message_.c_str());
    if (controller.quit_requested()) {
        mvprintw(atk_line++, layout.atk_x + 1, "Press Q again to quit");
    }

    if (controller.get_mode() == game::controller::Controller::Mode::SELECT) {
        auto* target = level->get_entity_at(controller.get_cursor());
        if (target) {
            mvprintw(atk_line++, layout.atk_x + 1, "Select target: %s",
                     target->get_name().c_str());
            if (target->get_team_id() == controller.active_team()) {
                mvprintw(atk_line++, layout.atk_x + 1, "Select: ok");
            } else {
                mvprintw(atk_line++, layout.atk_x + 1, "Select: blocked (enemy team)");
            }
        } else {
            mvprintw(atk_line++, layout.atk_x + 1, "Select target: none");
        }
    } else if (controller.get_mode() == game::controller::Controller::Mode::ATTACK
        && controller.selected_entity() != game::service::TurnService::kNoEntity) {
        auto* attacker = level->get_entity(controller.selected_entity());
        auto* target = level->get_entity_at(controller.get_cursor());
        auto* attacker_pos = attacker ? level->get_entity_position(attacker->get_id()) : nullptr;
        if (attacker && attacker_pos) {
            int dx = std::abs(controller.get_cursor().x - attacker_pos->x);
            int dy = std::abs(controller.get_cursor().y - attacker_pos->y);
            int dist = std::max(dx, dy);
            double chance = compute_hit_chance(*attacker, dist);
            game::service::VisionService vision;
            bool has_los = vision.has_line_of_fire(*level, *attacker_pos, controller.get_cursor());
            if (target) {
                mvprintw(atk_line++, layout.atk_x + 1, "Attack target: %s",
                         target->get_name().c_str());
                if (auto* hp = target->get_component<game::entity::components::HealthComponent>()) {
                    mvprintw(atk_line++, layout.atk_x + 1, "Target HP: %d/%d",
                             hp->get_current_hp(), hp->get_max_hp());
                }
            } else if (auto* cell_target = level->get_cell(controller.get_cursor())) {
                if (dynamic_cast<const game::repo::cells::IDestructibleCell*>(cell_target)) {
                    mvprintw(atk_line++, layout.atk_x + 1, "Attack target: %s",
                             std::string(cell_target->view_name()).c_str());
                } else {
                    mvprintw(atk_line++, layout.atk_x + 1, "Attack target: none");
                }
            } else {
                mvprintw(atk_line++, layout.atk_x + 1, "Attack target: none");
            }
            if (auto* wp = attacker->get_component<game::entity::components::WeaponComponent>()) {
                if (auto* weapon = wp->get_weapon()) {
                    mvprintw(atk_line++, layout.atk_x + 1, "Hit chance: %.0f%% Ammo: %d/%d",
                             chance * 100.0,
                             weapon->get_current_ammo(),
                             weapon->get_max_ammo());
                }
            }
            mvprintw(atk_line++, layout.atk_x + 1, "Line of fire: %s", has_los ? "clear" : "blocked");
        } else {
            mvprintw(atk_line++, layout.atk_x + 1, "Attack target: none");
        }
    }

    auto* cell = level->get_cell(controller.get_cursor());
    if (cell) {
        mvprintw(atk_line++, layout.atk_x + 1, "Cell: %s", std::string(cell->view_name()).c_str());
        if (auto* container = dynamic_cast<game::repo::cells::IItemContainer*>(cell)) {
            auto items = container->get_items();
            mvprintw(atk_line++, layout.atk_x + 1, "Cell items: %zu", items.size());
            if (!items.empty()) {
                std::ranges::sort(items, [](const auto* a, const auto* b) {
                    if (!a || !b) return a != nullptr;
                    return a->get_id() < b->get_id();
                });
                size_t idx = controller.cell_item_index() % items.size();
                mvprintw(atk_line++, layout.atk_x + 1, "Cell item [%zu/%zu]: %s",
                         idx + 1, items.size(), item_label(items[idx]).c_str());
            }
        }
    } else {
        mvprintw(atk_line++, layout.atk_x + 1, "Cell: none");
    }

    if (controller.selected_entity() != game::service::TurnService::kNoEntity) {
        auto* ent = level->get_entity(controller.selected_entity());
        if (ent) {
            if (auto* inv = ent->get_component<game::entity::components::InventoryComponent>()) {
                auto items = inv->get_items();
                if (!items.empty()) {
                    std::ranges::sort(items, [](const auto* a, const auto* b) {
                        if (!a || !b) return a != nullptr;
                        return a->get_id() < b->get_id();
                    });
                    size_t idx = controller.inv_item_index() % items.size();
                    mvprintw(atk_line++, layout.atk_x + 1, "Inv item [%zu/%zu]: %s",
                             idx + 1, items.size(), item_label(items[idx]).c_str());
                } else {
                    mvprintw(atk_line++, layout.atk_x + 1, "Inv item: none");
                }
            }
        }
    }

    int hint_y = layout.map_y + layout.map_h;
    if (hint_y < maxy) {
        mvprintw(hint_y, layout.map_x + 1,
                 "Keys: WASD move  Enter act  M mode  R reload  E end  P save  L load  Q quit");
        if (hint_y + 1 < maxy) {
            mvprintw(hint_y + 1, layout.map_x + 1,
                     "Loot: G pick  T drop  U use  [ ] cell  , . inv");
        }
    }
}

}
