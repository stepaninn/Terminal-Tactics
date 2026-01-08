#include "model/service/VisionService.h"

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
            if (cheb <= radius) map.set_visible(ax, ay, true);

            if (blocked) {
                if (lvl.is_blocks_vision(static_cast<size_t>(ax), static_cast<size_t>(ay))) {
                    next_start_slope = r_slope;
                    continue;
                }
                blocked = false;
                start_slope = next_start_slope;
            } else if (lvl.is_blocks_vision(static_cast<size_t>(ax), static_cast<size_t>(ay))) {
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
    if (pos.x >= lvl.get_width() || pos.y >= lvl.get_height()) return fov_map;
    if (!lvl.is_blocks_vision(pos.x, pos.y)) fov_map.set_visible(pos.x, pos.y, true);
    if (r <= 0) return fov_map;
    for (int i = 0; i < 8; ++i) {
        cast_light(fov_map, lvl, static_cast<int>(pos.x), static_cast<int>(pos.y), r, 1, 1.0, 0.0,
            multipliers[0][i], multipliers[1][i],
            multipliers[2][i], multipliers[3][i]);
    }

    return fov_map;
}

bool VisionService::has_line_of_fire(const game::repo::Level& lvl, game::Position from, game::Position to) {
    if (!lvl.in_bounds(from. x, from.y) || !lvl.in_bounds(to.x, to.y)) return false;

    int dx = to.x - from.x, dy = to.y - from.y;
    int nx = std::abs(dx), ny = std::abs(dy);
    int sx = (dx >=0) ? 1 : -1;
    int sy = (dy >=0) ? 1 : -1;

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

        if (!(x == to.x && y == to.y) && !lvl.can_shoot_through(x, y)) return false;
    }

    return true;
}

}
