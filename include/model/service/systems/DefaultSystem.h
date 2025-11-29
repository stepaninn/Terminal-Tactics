#ifndef INC_3_DEFAULTSYSTEM_H
#define INC_3_DEFAULTSYSTEM_H

#include "ISystem.h"
#include <memory>

namespace game {

class DefaultSystem : public ISystem {
public:
    explicit DefaultSystem(std::shared_ptr<EventBus> bus) noexcept
        : event_bus_(std::move(bus)) {}

    EventBus& get_event_bus() override { return *event_bus_; }

protected:
    std::shared_ptr<EventBus> event_bus_;
};

} // namespace game

#endif //INC_3_DEFAULTSYSTEM_H