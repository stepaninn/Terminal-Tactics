#ifndef INC_3_ISYSTEM_H
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
        virtual EventBus& get_event_bus() = 0;
    };

} // namespace game

#endif //INC_3_ISYSTEM_H