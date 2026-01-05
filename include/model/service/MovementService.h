#ifndef INC_3_TMP_SERVICE_MOVEMENTSERVICE_H
#define INC_3_TMP_SERVICE_MOVEMENTSERVICE_H

#include "model/repository/Level.h"
#include "ServiceBase.h"

#include <memory>

namespace game::service {

class MovementService : public ServiceBase {
public:
    explicit MovementService(std::shared_ptr<events::EventBus> bus = nullptr) : ServiceBase(std::move(bus)) {}

    /**
     * @brief Метод перемещения существа по уровню
     * @param level Уровень, на котором перемещается существо
     * @param id ID перемещающегося существа
     * @param to Позиция, в которую переместится существо
     * @return bool true, если перемещение успешно
     */
    [[nodiscard]] bool move(game::repo::Level& level, game::EntityId id, game::Position to);
};

}

#endif // INC_3_TMP_SERVICE_MOVEMENTSERVICE_H
