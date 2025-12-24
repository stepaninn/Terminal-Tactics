#include "model/service/MovementService.h"
#include "model/entity/components/MoveComponent.h"
#include "model/entity/components/TimePointsComponent.h"

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <utility>

namespace game::service {

bool MovementService::move(game::repo::Level& level, game::EntityId id, game::Position to) {
    auto* entity = level.get_entity(id);
    if (!entity) return false;

    auto* pos = entity->get_component<game::entity::components::PositionComponent>();
    auto* mv = entity->get_component<game::entity::components::MoveComponent>();
    auto* tp = entity->get_component<game::entity::components::TimePointsComponent>();
    if (!pos || !mv || !tp) return false;

    if (to.x >= level.get_field().rows() || to.y >= level.get_field().cols()) return false;
    auto* cell = level.get_cell(to);
    if (!cell || !cell->is_walkable()) return false;

    game::Position from = pos->get_position();
    int dx = static_cast<int>(to.x) - static_cast<int>(from.x);
    int dy = static_cast<int>(to.y) - static_cast<int>(from.y);
    int dist = std::max(std::abs(dx), std::abs(dy));
    int cost = dist * mv->get_step_cost();
    if (cost == 0) return true;
    if (tp->get_current_points() < cost) return false;
    if (tp->reduce_points(cost) != cost) return false;

    if (!level.move_entity(id, to)) return false;

    if (auto* eb = bus()) {
        auto ev = std::make_shared<events::MoveEvent>();
        ev->entity_id = id;
        ev->from = from;
        ev->to = to;
        eb->publish(std::move(ev));
    }
    return true;
}

}
