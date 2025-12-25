#ifndef INC_3_TMP_SERVICE_MOVEMENTSERVICE_H
#define INC_3_TMP_SERVICE_MOVEMENTSERVICE_H

#include "modelgit /repository/Level.h"
#include "model/entity/components/PositionComponent.h"
#include "ServiceBase.h"

#include <memory>

namespace game::service {

class MovementService : public ServiceBase {
public:
    explicit MovementService(std::shared_ptr<events::EventBus> bus = nullptr) : ServiceBase(std::move(bus)) {}

    [[nodiscard]] bool move(game::repo::Level& level, game::EntityId id, game::Position to);
};

}

#endif // INC_3_TMP_SERVICE_MOVEMENTSERVICE_H
