#ifndef MYGAMEPROJECT_EVENT_MT_H
#define MYGAMEPROJECT_EVENT_MT_H

#include <string>
#include <vector>
#include <optional>
#include "../../../types_mt.h"

namespace game::mt::service::events {

struct EventBase {
    virtual ~EventBase() = default;
};

struct MoveEvent final : EventBase {
    game::mt::id_t entity_id;
    game::mt::Position from;
    game::mt::Position to;
    std::optional<std::vector<Position>> path;
};

struct DamageEvent final : EventBase {
    game::mt::id_t attacker_id;
    game::mt::id_t target_id;
    int amount{0};
};

struct ShotMissedEvent final : EventBase {
    game::mt::id_t attacker_id;
    game::mt::id_t target_id;
};

struct ShotMissedAtCellEvent final : EventBase {
    game::mt::id_t attacker_id;
    game::mt::Position pos;
};

struct ItemPickedEvent final : EventBase {
    game::mt::id_t entity_id;
    game::mt::id_t item_id;
};

struct ItemDropEvent final : EventBase {
    game::mt::id_t entity_id;
    game::mt::id_t item_id;
};

struct EntityDiedEvent final : EventBase {
    game::mt::id_t entity_id;
    game::mt::id_t killer_id;
};

struct SaveRequestedEvent final : EventBase {
    std::string save_name;
};

struct ItemUsedEvent final : EventBase {
    game::mt::id_t user_id;
    game::mt::id_t target_id;
    game::mt::id_t item_id;
};

struct WallBrokenEvent final : EventBase {
    game::mt::Position pos;
};

}


#endif //MYGAMEPROJECT_EVENT_MT_H
