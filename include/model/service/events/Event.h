#ifndef INC_3_EVENT_H
#define INC_3_EVENT_H

#include <string>
#include "types.h"

namespace game {

struct EventBase {
    virtual ~EventBase() = default;
};

struct MoveEvent : EventBase {
    id_t entity_id;
    Position from;
    Position to;
};

struct DamageEvent : EventBase {
    id_t attacker_id;
    id_t target_id;
    int amount{0};
};

struct ItemPickedEvent : EventBase {
    id_t entity_id;
    id_t item_id;
};

struct EntityDiedEvent : EventBase {
    id_t entity_id;
    id_t killer_id;
};

struct SaveRequestedEvent : EventBase {
    std::string save_name;
};

struct ItemUsedEvent : EventBase {
    id_t user_id;
    id_t target_id;
    id_t item_id;
};

} // namespace game


#endif //INC_3_EVENT_H