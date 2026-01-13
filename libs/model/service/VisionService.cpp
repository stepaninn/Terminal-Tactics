#include "model/service/VisionService.h"

#include <unordered_set>

namespace game::service {

void VisionService::cast_light(VisibilityMap& map, const game::repo::Level& lvl, int x, int y, int radius, int row,
        double start_slope, double end_slope, int xx, int xy, int yx, int yy) {
    if (start_slope < end_slope) return;

    double next_start_slope = start_slope;

    for (int i = row; i <= radius; ++i) {
        bool blocked = false;
        for (int dx = -i, dy = -i; dx <= 0; ++dx) {
            double l_slope = (dx - 0.5) / (dy + 0.5);
            double r_slope = (dx + 0.5) / (dy - 0.5);

            if (start_slope < r_slope) continue;
            if (end_slope > l_slope) break;

            int sax = dx * xx + dy * xy;
            int say = dx * yx + dy * yy;

            int ax = x + sax;
            int ay = y + say;
            if (ax < 0 || ay < 0) continue;
            if (ax >= static_cast<int>(lvl.get_width()) || ay >= static_cast<int>(lvl.get_height())) continue;

            int cheb = std::max(std::abs(dx), std::abs(dy));
            if (cheb <= radius) map.set_visible({ax, ay}, true);

            if (blocked) {
                if (lvl.is_blocks_vision(ax, ay)) {
                    next_start_slope = r_slope;
                    continue;
                }
                blocked = false;
                start_slope = next_start_slope;
            } else if (lvl.is_blocks_vision(ax, ay)) {
                blocked = true;
                next_start_slope = r_slope;
                cast_light(map, lvl, x, y, radius, i + 1,
                    start_slope, l_slope, xx, xy, yx, yy);
            }
        }
        if (blocked) break;
    }
}

VisibilityMap VisionService::compute_fov(const game::repo::Level& lvl, Position pos, int r) {
    static int multipliers[4][8] = {
        {1, 0, 0, -1, -1, 0, 0, 1},
        {0, 1, -1, 0, 0, -1, 1, 0},
        {0, 1, 1, 0, 0, -1, -1, 0},
        {1, 0, 0, 1, -1, 0, 0, -1}
    };

    VisibilityMap fov_map(lvl.get_width(), lvl.get_height());
    if (!lvl.in_bounds(pos)) return fov_map;
    if (!lvl.is_blocks_vision(pos)) {
        fov_map.set_visible(pos, true);
    }
    if (r <= 0) return fov_map;
    for (int i = 0; i < 8; ++i) {
        cast_light(fov_map, lvl, pos.x, pos.y, r, 1, 1.0, 0.0,
            multipliers[0][i], multipliers[1][i],
            multipliers[2][i], multipliers[3][i]);
    }

    return fov_map;
}

bool VisionService::has_line_of_fire(const game::repo::Level& lvl, game::Position from, game::Position to) {
    if (!lvl.in_bounds(from) || !lvl.in_bounds(to)) return false;
    if (from == to) return true;

    int dx = to.x - from.x, dy = to.y - from.y;
    int nx = std::abs(dx), ny = std::abs(dy);
    int sx = (dx >= 0) ? 1 : -1;
    int sy = (dy >= 0) ? 1 : -1;

    int x = from.x, y = from.y;
    int ix = 0, iy = 0;

    while (ix <= nx || iy <= ny) {
        long long a = (1 + 2 * ix) * ny; // t_x = (ix + 0.5)/nx v t_y = (iy + 0.5)/ny
        long long b = (1 + 2 * iy) * nx; // (2ix + 1)*ny v (2iy + 1)*nx

        if (a < b) {
            x += sx; ++ix;
        } else if (a > b) {
            y += sy; ++iy;
        } else {
            if (lvl.in_bounds(x + sx, y) && !lvl.can_shoot_through(x + sx, y)) return false;
            if (lvl.in_bounds(x, y + sy) && !lvl.can_shoot_through(x, y + sy)) return false;
            x += sx; y += sy; ++ix; ++iy;
        }

        if (!lvl.in_bounds(x, y)) return false;

        if (x == to.x && y == to.y) return true;
        if (!lvl.can_shoot_through(x, y)) return false;
    }

    return false;
}

void VisionService::update_unit_fov(game::service::World& w, game::EntityId id) {
    auto* level = w.get_level();
    if (!level) return;
    auto* ent = w.get_entity(id);
    if (!ent) return;
    auto* pos = level->get_entity_position(id);
    if (!pos) return;

    auto* vision = ent->get_component<game::entity::components::VisionComponent>();
    if (!vision) return;

    int radius = vision->get_vision_radius();

    auto* ufov = w.unit_visibility(id);
    *ufov = compute_fov(*level, *pos, radius);
}

void VisionService::rebuild_team_visible(game::service::World& w, game::TeamId team) {
    auto* level = w.get_level();
    if (!level) return;

    auto* team_vis = w.team_visibility(team);
    if (!team_vis) return;
    team_vis->resize(level->get_width(), level->get_height());
    team_vis->clear_visible();

    for (auto id : w.get_team_entities(team)) {
        const auto* ufov = w.unit_visibility(id);
        if (!ufov) continue;
        size_t rows = std::min(team_vis->rows(), ufov->rows());
        size_t cols = std::min(team_vis->cols(), ufov->cols());
        for (size_t x = 0; x < rows; ++x) {
            for (size_t y = 0; y < cols; ++y) {
                if (ufov->is_visible(x, y)) team_vis->set_visible(x, y, true);
            }
        }
    }

    update_team_explored(w, team);
}

void VisionService::update_team_explored(game::service::World& w, game::TeamId team) {
    auto* level = w.get_level();
    if (!level) return;
    const auto* team_vis = w.team_visibility(team);
    if (!team_vis) return;

    auto* explored = w.team_exploration(team);
    if (!explored) return;
    explored->resize(level->get_width(), level->get_height());
    for (size_t x = 0; x < team_vis->rows(); ++x) {
        for (size_t y = 0; y < team_vis->cols(); ++y) {
            if (team_vis->is_visible(x, y)) explored->set_explored(x, y, true);
        }
    }
}

void VisionService::rebuild_all_fov(game::service::World& w) {
    auto* level = w.get_level();
    if (!level) return;

    auto entities = w.get_all_entities();
    for (const auto* ent : entities) {
        if (!ent) continue;
        if (!ent->get_component<game::entity::components::VisionComponent>()) continue;
        update_unit_fov(w, ent->get_id());
    }

    std::unordered_set<game::TeamId> teams;
    teams.reserve(entities.size());
    for (const auto* ent : entities) {
        if (!ent) continue;
        teams.insert(ent->get_team_id());
    }
    for (auto team : teams) rebuild_team_visible(w, team);
}

std::vector<EntityId> VisionService::visible_entities(game::service::World& w,
                                                      game::EntityId observer_id) {
    auto* level = w.get_level();
    if (!level) return {};
    auto* observer = w.get_entity(observer_id);
    if (!observer) return {};
    auto* pos = level->get_entity_position(observer_id);
    if (!pos) return {};

    auto* vision = observer->get_component<game::entity::components::VisionComponent>();
    if (!vision) return {};
    int radius = vision->get_vision_radius();
    if (vision->is_sees_items_only()) radius = 1; // чтобы фуражир видел хоть что-то

    auto fov = compute_fov(*level, *pos, radius);

    std::vector<EntityId> res;
    auto entites = level->get_entities_radius(*pos, radius);
    res.reserve(entites.size());
    for (const auto* ent : entites) {
        if (!ent || ent->get_id() == observer_id) continue;
        auto* ent_pos = level->get_entity_position(ent->get_id());
        if (!ent_pos) continue;
        if (fov.is_visible(*ent_pos)) res.push_back(ent->get_id());
    }
    return res;
}

}
