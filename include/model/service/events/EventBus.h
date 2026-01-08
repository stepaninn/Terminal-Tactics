#ifndef MYGAMEPROJECT_EVENTBUS_H
#define MYGAMEPROJECT_EVENTBUS_H

#include "Event.h"
#include <memory>
#include <map>
#include <queue>
#include <typeindex>
#include <vector>
#include <functional>
#include <utility>

namespace game::service::events {

class EventBus {
public:
    template<typename T, typename Func>
    void subscribe(Func&& handler) {
        auto key = std::type_index(typeid(T));

        std::function<void(std::shared_ptr<EventBase>)> wrapper =
            [handler = std::forward<Func>(handler)](const std::shared_ptr<EventBase>& ev_base) {
                auto ev = std::static_pointer_cast<T>(ev_base);
                handler(ev);
        };

        handlers_[key].push_back(std::move(wrapper));
    }

    void publish(std::shared_ptr<EventBase> ev);

    void process();

private:
    std::map<std::type_index, std::vector<std::function<void(std::shared_ptr<EventBase>)>>> handlers_;
    std::queue<std::shared_ptr<EventBase>> event_queue_;
};

}


#endif //MYGAMEPROJECT_EVENTBUS_H
