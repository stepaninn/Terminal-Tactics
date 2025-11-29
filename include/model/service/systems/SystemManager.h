#ifndef INC_3_SYSTEMMANAGER_H
#define INC_3_SYSTEMMANAGER_H

#include "ISystem.h"

#include <vector>
#include <memory>

#include "EventBus.h"

namespace game {

class SystemManager {
public:
    SystemManager()
        : event_bus_(std::make_shared<EventBus>()) {}

    EventBus& get_event_bus() { return *event_bus_; }

    const std::vector<std::shared_ptr<ISystem>>& get_systems() const noexcept { return systems_; }

    void set_systems(const std::vector<std::shared_ptr<ISystem>>& sys);

    void add_system(std::shared_ptr<ISystem> sys);

private:
    std::shared_ptr<EventBus> event_bus_;
    std::vector<std::shared_ptr<ISystem>> systems_;
};

} // namespace game

#endif //INC_3_SYSTEMMANAGER_H