#ifndef INC_3_ISYSTEM_H[[nodiscard]]
#define INC_3_ISYSTEM_H

#include <memory>
#include "model/repository/Level.h"
#include "types.h"
#include "EventBus.h"

namespace game {

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void update(std::shared_ptr<Level> lvl) = 0;
    [[nodiscard]] virtual EventBus& get_event_bus() = 0;
};

class DefaultSystem : public ISystem {
public:
    explicit DefaultSystem(std::shared_ptr<EventBus> bus) noexcept
        : event_bus_(std::move(bus)) {}

    [[nodiscard]] EventBus& get_event_bus() override { return *event_bus_; }

protected:
    std::shared_ptr<EventBus> event_bus_;
};


} // namespace game

#endif //INC_3_ISYSTEM_H