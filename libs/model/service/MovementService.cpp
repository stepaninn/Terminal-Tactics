#include "model/service/MovementService.h"
#include "model/entity/components/MoveComponent.h"
#include "model/entity/components/TimePointsComponent.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <queue>
#include <stdexcept>

namespace game::service {

bool MovementService::move(game::repo::Level& level, game::EntityId id, game::Position to) const {
    auto* entity = level.get_entity(id);
    if (!entity) return false;

    auto* mv = entity->get_component<game::entity::components::MoveComponent>();
    auto* tp = entity->get_component<game::entity::components::TimePointsComponent>();
    if (!mv || !tp) return false;

    std::vector<Position> path = find_path(level, id, to);

    int cost = static_cast<int>(path.size()) * mv->get_step_cost();
    if (cost == 0) return false;
    if (tp->get_current_points() < cost) return false;
    if (tp->reduce_points(cost) != cost) return false;

    auto* from = level.get_entity_position(id);

    if (!level.move_entity(id, to)) return false;

    if (auto* eb = bus()) {
        auto ev = std::make_shared<events::MoveEvent>();
        ev->entity_id = id;
        ev->from = *from;
        ev->to = to;
        ev->path = path;
        eb->publish(std::move(ev));
    }
    return true;
}

std::vector<game::Position> MovementService::find_path(const game::repo::Level& level,
                                                       game::EntityId id,
                                                       game::Position to) {
    if (!level.check_entity(id)) return {};

    auto* from = level.get_entity_position(id);
    if (!from || *from == to) return {};

    auto* target_cell = level.get_cell(to);
    if (!target_cell || !target_cell->is_walkable()) return {};

    size_t rows = level.get_field().rows();
    size_t cols = level.get_field().cols();
    if (!level.in_bounds(to)) return {};
    if (!level.in_bounds(*from)) return {};

    std::vector<std::vector<bool>> occupied(rows, std::vector<bool>(cols, false));
    for (auto& e : level.get_entities()) {
        if (e->get_id() == id) continue;
        auto pos = level.get_entity_position(e->get_id());
        if (!pos) throw std::logic_error("No position for entity!");
        if (!level.in_bounds(*pos)) {
            throw std::logic_error("Entity position out of bounds!");
        }
        occupied[static_cast<size_t>(pos->x)][static_cast<size_t>(pos->y)] = true;
    }

    if (occupied[static_cast<size_t>(to.x)][static_cast<size_t>(to.y)]) return {};

    // bfs
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::vector<std::vector<game::Position>> parent(
        rows, std::vector<game::Position>(cols, {-1, -1}));

    std::queue<game::Position> q;
    visited[static_cast<size_t>(from->x)][static_cast<size_t>(from->y)] = true;
    q.push(*from);

    const int dirs[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},             { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    };

    while (!q.empty()) {
        auto cur = q.front();
        q.pop();

        for (auto& d : dirs) {
            int nx = cur.x + d[0];
            int ny = cur.y + d[1];
            if (nx < 0 || ny < 0) continue;
            auto ux = static_cast<size_t>(nx);
            auto uy = static_cast<size_t>(ny);
            if (ux >= rows || uy >= cols) continue;
            if (visited[ux][uy] || occupied[ux][uy]) continue;

            auto* cell = level.get_cell({nx, ny});
            if (!cell || !cell->is_walkable()) continue;

            // запрет идти по диагонали
            if (d[0] != 0 && d[1] != 0) {
                auto* c1 = level.get_cell({cur.x, ny});
                auto* c2 = level.get_cell({nx, cur.y});
                if (!c1 || !c2 || !c1->is_walkable() || !c2->is_walkable()) continue;
            }

            visited[ux][uy] = true;
            parent[ux][uy] = cur;
            q.push({nx, ny});
        }
    }

    if (!visited[static_cast<size_t>(to.x)][static_cast<size_t>(to.y)]) return {};

    // воссатновление пути
    std::vector<game::Position> path;
    for (auto cur = to; !(cur.x == from->x && cur.y == from->y);
         cur = parent[static_cast<size_t>(cur.x)][static_cast<size_t>(cur.y)]) {
        path.push_back(cur);
    }
    std::ranges::reverse(path);
    return path;
}

}
