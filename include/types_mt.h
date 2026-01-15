#ifndef MYGAMEPROJECT_TYPES_MT_H
#define MYGAMEPROJECT_TYPES_MT_H

#include <algorithm>
#include <cstdint>

namespace game::mt {

using id_t = uint64_t;
using EntityId = uint64_t;
using LevelId = uint64_t;
using ItemId = uint64_t;
using TeamId = uint64_t;

enum class CellType {
    FLOOR,
    WALL,
    GLASS,
    STASH
};

struct Position {
    int x = 0;
    int y = 0;
    bool operator==(game::mt::Position to) const { return x == to.x && y == to.y; }
};

enum class EnemyType {
    WILD,
    INTELLIGENT,
    FORAGER
};

struct Damage {
    int min_dmg = 1;
    int max_dmg = 1;

    constexpr Damage(int mn, int mx) : min_dmg(std::max(1, mn)), max_dmg(std::max(1, mx)) {
        if (min_dmg > max_dmg) std::swap(min_dmg, max_dmg);
    }
};

enum class AmmoType {
    SHOTGUN,
    RIFLE,
    PISTOL,
    SNIPER
};

}

#endif
