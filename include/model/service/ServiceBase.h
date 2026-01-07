#ifndef MYGAMEPROJECT_SERVICEBASE_H
#define MYGAMEPROJECT_SERVICEBASE_H

#include "events/EventBus.h"

#include <memory>

namespace game::service {

class ServiceBase {
public:
    explicit ServiceBase(std::shared_ptr<events::EventBus> bus = nullptr) : event_bus_(std::move(bus)) {}

protected:
    [[nodiscard]] events::EventBus* bus() const noexcept { return event_bus_.get(); }

private:
    std::shared_ptr<events::EventBus> event_bus_;
};

}

#endif // MYGAMEPROJECT_SERVICEBASE_H
