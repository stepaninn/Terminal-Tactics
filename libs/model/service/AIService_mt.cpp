#include "model/service/AIService_mt.h"

#include "model/entity/components/AIComponent_mt.h"
#include "model/entity/components/InventoryComponent_mt.h"
#include "model/entity/components/TimePointsComponent_mt.h"

#include "model/repository/cells/Cell_mt.h"
#include "model/repository/cells/ItemContainer_mt.h"
#include "model/repository/cells/Stash_mt.h"
#include "model/service/EntityLock_mt.h"

#include <algorithm>
#include <array>

namespace game::mt::service {

void AIService::act_team(World& w, TeamId team_id) {
    auto ids = w.get_team_entities(team_id);
    for (auto id : ids) {
        act_entity(w, id);
    }
}

void AIService::act_entity(World& w, EntityId id) {
    auto ent = w.get_entity(id);
    if (!ent) return;

    auto* tp = ent->get_component<entity::components::TimePointsComponent>();
    auto* ai = ent->get_component<entity::components::AIComponent>();

    if (!tp || !ai) return;

    if (tp->get_current_points() <= 0) return;

    auto choice = choose_best_action(w, id);
    execute(w, id, choice);
}

AIService::Choice AIService::choose_best_action(World& w, EntityId id) {
    Choice best{};
    best.kind = ActionKind::WAIT;
    best.score = -1e9;

    auto* level = w.get_level();
    auto ent = w.get_entity(id);
    if (!level || !ent) return best;

    auto* ai = ent->get_component<entity::components::AIComponent>();
    if (!ai) return best;

    auto behavior = ai->get_behavior();

    auto pos = level->get_entity_position(id);
    if (!pos) return best;

    auto* inv = ent->get_component<entity::components::InventoryComponent>();

    bool inv_exists = (inv != nullptr);
    bool inv_has_items = inv_exists && (inv->size() > 0);
    bool inv_full = inv_exists &&
        (inv->size() >= static_cast<size_t>(inv->get_capacity()) || inv->get_weight() >= inv->get_max_weight());

    auto enemy = find_nearest_enemy(w, id);

    bool can_pick_here = false;
    if (inv_exists) {
        if (auto* cell = level->get_cell(*pos)) {
            if (auto* cont = dynamic_cast<game::mt::repo::cells::IItemContainer*>(cell)) {
                auto items = cont->get_items();
                if (!items.empty() && items.front() && inv->can_add(*items.front())) {
                    can_pick_here = true;
                }
            }
        }
    }

    bool can_deposit_here = false;
    if (inv_has_items) {
        if (auto* cell = level->get_cell(*pos)) {
            if (dynamic_cast<game::mt::repo::cells::Stash*>(cell)) {
                can_deposit_here = true;
            }
        }
    }

    std::optional<Position> nearest_stash;
    if (inv_full || inv_has_items) {
        auto stashes = find_stashes(*level);
        if (!stashes.empty()) {
            nearest_stash = *std::ranges::min_element(stashes,
                                                      [&](Position a, Position b) {
                                                          return get_distance(*pos, a) < get_distance(*pos, b);
                                                      });
        }
    }

    std::optional<Position> nearest_item_cell;
    if (inv_exists && !inv_full) {
        auto item_cells = find_item_cells(*level);
        if (!item_cells.empty()) {
            nearest_item_cell = *std::ranges::min_element(item_cells,
                                                          [&](Position a, Position b) {
                                                              return get_distance(*pos, a) < get_distance(*pos, b);
                                                          });
        }
    }

    std::vector<Choice> options;
    options.reserve(10);

    // атака
    if (enemy) {
        if (is_adjacent(*pos, enemy->pos)) {
            Choice c;
            c.kind = ActionKind::MELEE;
            c.enemy = enemy;

            double base = 120.0;
            base += 10.0 / (1 + enemy->dist);
            c.score = base * weight(behavior, c.kind);
            options.push_back(c);
        }

        if (enemy->line_of_fire && game::mt::service::CombatService::can_shoot(*ent)) {
            Choice c;
            c.kind = ActionKind::SHOOT;
            c.enemy = enemy;

            double base = 140.0;
            base += 5.0 / (1 + enemy->dist);
            c.score = base * weight(behavior, c.kind);
            options.push_back(c);
        }

        {
            Choice c;
            c.kind = ActionKind::MOVETOENEMY;
            c.enemy = enemy;
            c.move_to = enemy->pos;

            double base = 60.0;
            base += 20.0 / (1 + enemy->dist);
            c.score = base * weight(behavior, c.kind);
            options.push_back(c);
        }
    }

    // предметы
    if (can_deposit_here) {
        Choice c;
        c.kind = ActionKind::DEPOSITHERE;
        double base = 130.0;
        c.score = base * weight(behavior, c.kind);
        options.push_back(c);
    }

    if (can_pick_here) {
        Choice c;
        c.kind = ActionKind::PICKUPHERE;
        double base = 110.0;
        c.score = base * weight(behavior, c.kind);
        options.push_back(c);
    }

    if (nearest_stash && inv_full) {
        Choice c;
        c.kind = ActionKind::MOVETOSTASH;
        c.move_to = *nearest_stash;

        double base = 90.0;
        base += 15.0 / (1 + get_distance(*pos, *nearest_stash));
        c.score = base * weight(behavior, c.kind);
        options.push_back(c);
    }

    if (nearest_item_cell && inv_exists && !inv_full) {
        Choice c;
        c.kind = ActionKind::MOVETOITEM;
        c.move_to = *nearest_item_cell;

        double base = 70.0;
        base += 10.0 / (1 + get_distance(*pos, *nearest_item_cell));
        c.score = base * weight(behavior, c.kind);
        options.push_back(c);
    }

    {
        Choice c;
        c.kind = ActionKind::WANDER;
        double base = 15.0;
        c.score = base * weight(behavior, c.kind);
        options.push_back(c);
    }

    std::uniform_real_distribution<double> noise(0.95, 1.05);
    {
        std::lock_guard<std::mutex> lock(rng_mutex_);
        for (auto& c : options) c.score *= noise(rng_);
    }

    for (const auto& c : options) {
        if (c.score > best.score) best = c;
    }

    return best;
}

void AIService::execute(World& w, EntityId id, const Choice& c) {
    auto* level = w.get_level();
    if (!level) return;

    switch (c.kind) {
        case ActionKind::SHOOT:
            if (c.enemy) {
                (void)ctx_.combat.try_shoot(*level, id, c.enemy->id);
            }
            break;

        case ActionKind::MELEE:
            if (c.enemy) {
                (void)ctx_.combat.melee_attack(*level, id, c.enemy->id);
            }
            break;

        case ActionKind::PICKUPHERE:
            (void)pickup_one_here(*level, id);
            break;

        case ActionKind::DEPOSITHERE:
            (void)deposit_all_here(*level, id);
            break;

        case ActionKind::MOVETOENEMY:
        case ActionKind::MOVETOITEM:
        case ActionKind::MOVETOSTASH:
            if (c.move_to) {
                (void)move_one_step(w, id, *c.move_to);
            }
            break;

        case ActionKind::WANDER:
            (void)random_step(w, id);
            break;

        case ActionKind::WAIT:
        default:
            break;
    }
}

double AIService::weight(entity::components::AIBehavior behavior, ActionKind kind) {
    using AIBehavior = entity::components::AIBehavior;

    switch (behavior) {
        case AIBehavior::WILD:
            switch (kind) {
                case ActionKind::MELEE:        return 1.4;
                case ActionKind::SHOOT:        return 0.0;
                case ActionKind::MOVETOENEMY:  return 1.2;
                case ActionKind::PICKUPHERE:   return 0.1;
                case ActionKind::DEPOSITHERE:  return 0.1;
                case ActionKind::MOVETOITEM:   return 0.2;
                case ActionKind::MOVETOSTASH:  return 0.2;
                case ActionKind::WANDER:       return 1.0;
                case ActionKind::WAIT:         return 1.0;
            }
            break;

        case AIBehavior::INTELLIGENT:
            switch (kind) {
                case ActionKind::MELEE:        return 0.7;
                case ActionKind::SHOOT:        return 1.4;
                case ActionKind::MOVETOENEMY:  return 1.0;
                case ActionKind::PICKUPHERE:   return 0.2;
                case ActionKind::DEPOSITHERE:  return 0.2;
                case ActionKind::MOVETOITEM:   return 0.3;
                case ActionKind::MOVETOSTASH:  return 0.3;
                case ActionKind::WANDER:       return 0.9;
                case ActionKind::WAIT:         return 1.0;
            }
            break;

        case AIBehavior::FORAGER:
            switch (kind) {
                case ActionKind::MELEE:        return 0.0;
                case ActionKind::SHOOT:        return 0.0;
                case ActionKind::MOVETOENEMY:  return 0.1;
                case ActionKind::PICKUPHERE:   return 1.6;
                case ActionKind::DEPOSITHERE:  return 1.7;
                case ActionKind::MOVETOITEM:   return 1.3;
                case ActionKind::MOVETOSTASH:  return 1.2;
                case ActionKind::WANDER:       return 1.0;
                case ActionKind::WAIT:         return 1.0;
            }
            break;
    }

    return 1.0;
}

int AIService::get_distance(Position a, Position b) {
    return std::max(std::abs(a.x - b.x), std::abs(a.y - b.y));
}

bool AIService::is_adjacent(Position a, Position b) {
    return get_distance(a, b) <= 1;
}

std::optional<AIService::Target> AIService::find_nearest_enemy(World& w, EntityId observer_id) const {
    auto* level = w.get_level();
    if (!level) return std::nullopt;

    auto observer = w.get_entity(observer_id);
    if (!observer) return std::nullopt;

    auto pos = level->get_entity_position(observer_id);
    if (!pos) return std::nullopt;

    auto visible = ctx_.vision.visible_entities(w, observer_id);

    std::optional<Target> best;
    for (auto id : visible) {
        auto ent = w.get_entity(id);
        if (!ent) continue;
        if (ent->get_team_id() == observer->get_team_id()) continue;

        auto ep = level->get_entity_position(id);
        if (!ep) continue;

        int d = get_distance(*pos, *ep);
        bool lof = ctx_.vision.has_line_of_fire(*level, *pos, *ep);

        if (!best || d < best->dist) {
            best = Target{id, *ep, d, lof};
        }
    }
    return best;
}

std::vector<Position> AIService::find_stashes(const game::mt::repo::Level& level) {
    std::vector<Position> res;
    for (int x = 0; x < static_cast<int>(level.get_width()); ++x) {
        for (int y = 0; y < static_cast<int>(level.get_height()); ++y) {
            auto* cell = level.get_cell({x, y});
            if (cell && dynamic_cast<const game::mt::repo::cells::Stash*>(cell)) {
                res.push_back({x, y});
            }
        }
    }
    return res;
}

std::vector<Position> AIService::find_item_cells(const game::mt::repo::Level& level) {
    std::vector<Position> res;
    for (int x = 0; x < static_cast<int>(level.get_width()); ++x) {
        for (int y = 0; y < static_cast<int>(level.get_height()); ++y) {
            auto* cell = level.get_cell({x, y});
            auto* container = dynamic_cast<const game::mt::repo::cells::IItemContainer*>(cell);
            if (!container) continue;
            if (container->size() == 0) continue;

            auto items = container->get_items();
            if (items.empty() || !items.front()) continue;

            res.push_back({x, y});
        }
    }
    return res;
}

bool AIService::move_one_step(World& w, EntityId id, Position to) const {
    auto* level = w.get_level();
    if (!level) return false;

    std::vector<Position> path;
    if (level->get_entity_at(to)) {
        path = ctx_.move.find_path_without_target(*level, id, to);
    } else {
        path = ctx_.move.find_path(*level, id, to);
    }
    if (path.empty()) return false;

    return ctx_.move.move(*level, id, path.front());
}

bool AIService::random_step(World& w, EntityId id) {
    auto* level = w.get_level();
    if (!level) return false;

    auto pos = level->get_entity_position(id);
    if (!pos) return false;

    const int dirs[8][2] ={
        {-1, 0}, {0, 1}, {1, 0}, {0, -1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

    std::array<int, 8> order{0, 1, 2, 3, 4, 5, 6, 7};
    {
        std::lock_guard<std::mutex> lock(rng_mutex_);
        std::ranges::shuffle(order, rng_);
    }

    return std::ranges::any_of(order, [&](int idx) {
        Position next{pos->x + dirs[idx][0], pos->y + dirs[idx][1]};
        if (!level->in_bounds(next)) return false;

        auto* cell = level->get_cell(next);
        if (!cell || !cell->is_walkable()) return false;

        return ctx_.move.move(*level, id, next);
    });
}

bool AIService::pickup_one_here(game::mt::repo::Level& level, game::mt::EntityId id) {
    auto lock = EntityLockPool::instance().lock_entity(id);
    auto ent = level.get_entity(id);
    if (!ent) return false;

    auto* inv = ent->get_component<entity::components::InventoryComponent>();
    if (!inv) return false;

    auto pos = level.get_entity_position(id);
    if (!pos) return false;

    auto* cell = level.get_cell(*pos);
    auto* cont = dynamic_cast<game::mt::repo::cells::IItemContainer*>(cell);
    if (!cont) return false;

    auto items = cont->get_items();
    if (items.empty()) return false;

    auto item = items.front();
    if (!inv->can_add(*item)) return false;

    auto picked = cont->remove_by_id(item->get_id());
    if (!picked) return false;

    auto picked_id = picked->get_id();
    inv->add(picked);
    if (!inv->get_item(picked_id)) {
        if (auto rejected = cont->add(std::move(picked))) {
            inv->add(std::move(rejected));
        }
        return false;
    }

    return true;
}

bool AIService::deposit_all_here(game::mt::repo::Level& level, game::mt::EntityId id) {
    auto lock = EntityLockPool::instance().lock_entity(id);
    auto ent = level.get_entity(id);
    if (!ent) return false;

    auto* inv = ent->get_component<entity::components::InventoryComponent>();
    if (!inv || inv->size() == 0) return false;

    auto pos = level.get_entity_position(id);
    if (!pos) return false;

    auto* cell = level.get_cell(*pos);
    auto* stash = dynamic_cast<game::mt::repo::cells::Stash*>(cell);
    if (!stash) return false;

    return drop_all_to_cell(*stash, *inv);
}

bool AIService::drop_all_to_cell(game::mt::repo::cells::IItemContainer& cell,
                                 entity::components::InventoryComponent& inv) {
    auto items = inv.get_items();
    bool moved = false;

    for (const auto& item : items) {
        if (!item) continue;

        auto removed = inv.remove_by_id(item->get_id());

        if (auto rejected = cell.add(std::move(removed))) {
            inv.add(std::move(rejected));
        } else {
            moved = true;
        }
    }

    return moved;
}

}
