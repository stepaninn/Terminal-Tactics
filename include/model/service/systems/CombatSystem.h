#ifndef INC_3_COMBATSYSTEM_H
#define INC_3_COMBATSYSTEM_H

#include "DefaultSystem.h"
#include "types.h"

namespace game {

class CombatSystem : public DefaultSystem {
public:
    explicit CombatSystem(std::shared_ptr<EventBus> bus) : DefaultSystem(bus) {}

    bool attack(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target);
    void update(std::shared_ptr<Level> lvl) override;

};

} // namespace game

#endif //INC_3_COMBATSYSTEM_H