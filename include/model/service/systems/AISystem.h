#ifndef INC_3_AISYSTEM_H
#define INC_3_AISYSTEM_H

#include "ISystem.h"

namespace game {

class AISystem : public DefaultSystem {
public:
    explicit AISystem(std::shared_ptr<EventBus> bus) : DefaultSystem(bus) {}

    void act(std::shared_ptr<Entity> ent);
    void update(std::shared_ptr<Level> lvl) override;
};

} // namespace game

#endif //INC_3_AISYSTEM_H