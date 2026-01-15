#include "model/service/events/EventBus_mt.h"

namespace game::mt::service::events {

void EventBus::publish(std::shared_ptr<EventBase> ev) {
    if (!ev) return;
    event_queue_.push(std::move(ev));
}

void EventBus::process() {
    while (!event_queue_.empty()) {
        auto ev = std::move(event_queue_.front());
        event_queue_.pop();
        const auto* raw = ev.get();
        if (!raw) continue;
        auto it = handlers_.find(std::type_index(typeid(*raw)));
        if (it == handlers_.end()) continue;
        for (const auto& handler : it->second) {
            handler(ev);
        }
    }
}

}
