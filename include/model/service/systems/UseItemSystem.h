#ifndef INC_3_USEITEMSYSTEM_H
#define INC_3_USEITEMSYSTEM_H

#include "DefaultSystem.h"

namespace game {

class UseItemSystem : public DefaultSystem {
public:
    explicit UseItemSystem(std::shared_ptr<EventBus> bus) : DefaultSystem(bus) {}

    bool use_item(std::shared_ptr<Entity> ent,
                  std::shared_ptr<Entity> target,
                  std::shared_ptr<Item> item);

    void update(std::shared_ptr<Level> lvl) override;
};

} // namespace game

#endif //INC_3_USEITEMSYSTEM_H