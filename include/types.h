#ifndef INC_3_TYPES_H
#define INC_3_TYPES_H

#include <string>
#include <stdexcept>
#include <algorithm>

namespace game {

using id_t = size_t;
using EntityId = size_t;
using LevelId = size_t;
using ItemId = size_t;
using TeamId = size_t;

enum class CellType {
    FLOOR,
    WALL,
    GLASS,
    STASH
};

struct Position {
    size_t x = 0;
    size_t y = 0;
};

enum class EnemyType {
    WILD,
    INTELLIGENT,
    FORAGER
};

struct Damage {
    int min_dmg = 0;
    int max_dmg = 0;

    constexpr Damage(int mn, int mx) : min_dmg(mn), max_dmg(mx) {
        if (min_dmg > max_dmg) std::swap(min_dmg, max_dmg);
        if (min_dmg < 0 || max_dmg < 0) throw std::invalid_argument("Damage must be non negative");
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