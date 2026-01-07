#ifndef MYGAMEPROJECT_TYPES_H
#define MYGAMEPROJECT_TYPES_H

#include <algorithm>
#include <cstdint>

namespace game {

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
    size_t x = 0;
    size_t y = 0;
    bool operator==(game::Position to) const { return x == to.x && y == to.y; }
};

enum class EnemyType {
    WILD,
    INTELLIGENT,
    FORAGER
};

struct Damage {
    int min_dmg = 0;
    int max_dmg = 0;

    constexpr Damage(int mn, int mx) : min_dmg(std::max(0, mn)), max_dmg(std::max(0, mx)) {
        if (min_dmg > max_dmg) std::swap(min_dmg, max_dmg);
    }
};

enum class AIState {
    NEUTRAL,
    AGGRESSIVE,
    PATROLING
};

enum class AmmoType {
    SHOTGUN,
    RIFLE,
    PISTOL,
    SNIPER
};

}

#endif