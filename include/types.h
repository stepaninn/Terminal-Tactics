#ifndef INC_3_TYPES_H
#define INC_3_TYPES_H

#include <string>

namespace game {

using id_t = size_t;

enum class ActionResult {
    Ok,
    Failed,
    NotFound,
    NoSpace,
    OutOfBounds,
    InvalidArgument,
    AlreadyExists
};

enum class CellType {
    FLOOR,
    OPEN_DOOR,
    CLOSED_DOOR,
    WALL,
    GLASS,
    STASH
};

struct Position {
    int x = 0;
    int y = 0;
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