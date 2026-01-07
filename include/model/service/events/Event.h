#ifndef MYGAMEPROJECT_EVENT_H
#define MYGAMEPROJECT_EVENT_H

#include <string>
#include <vector>
#include <optional>
#include "../../../types.h"

namespace game::service::events {

struct EventBase {
    virtual ~EventBase() = default;
};

struct MoveEvent final : EventBase {
    game::id_t entity_id;
    game::Position from;
    game::Position to;
    std::optional<std::vector<Position>> path;
};

struct DamageEvent final : EventBase {
    game::id_t attacker_id;
    game::id_t target_id;
    int amount{0};
};

struct ShotMissedEvent final : EventBase {
    game::id_t attacker_id;
    game::id_t target_id;
};

struct ItemPickedEvent final : EventBase {
    game::id_t entity_id;
    game::id_t item_id;
};

struct EntityDiedEvent final : EventBase {
    game::id_t entity_id;
    game::id_t killer_id;
};

struct SaveRequestedEvent final : EventBase {
    std::string save_name;
};

struct ItemUsedEvent final : EventBase {
    game::id_t user_id;
    game::id_t target_id;
    game::id_t item_id;
};

}


#endif //MYGAMEPROJECT_EVENT_H
