#ifndef INC_3_MOVEMENTSYSTEM_H
#define INC_3_MOVEMENTSYSTEM_H

#include "ISystem.h"
#include "types.h"
#include <vector>

namespace game {

class MovementSystem : public DefaultSystem {
public:
    explicit MovementSystem(std::shared_ptr<EventBus> bus) : DefaultSystem(bus) {}

    [[nodiscard]] bool move(std::shared_ptr<Entity> entity, const Position& pos);

    [[nodiscard]] std::vector<Position> find_path(std::shared_ptr<Entity> entity, const Position& to, const std::shared_ptr<Level> lvl);

    void update(std::shared_ptr<Level> lvl) override;
};

} // namespace game

#endif //INC_3_MOVEMENTSYSTEM_H