#ifndef INC_3_TYPES_H
#define INC_3_TYPES_H

#include <string>

namespace game {

using id_t = size_t;

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

} // namespace game

#endif