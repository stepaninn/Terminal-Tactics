#ifndef MYGAMEPROJECT_MOVEMENTSERVICE_H
#define MYGAMEPROJECT_MOVEMENTSERVICE_H

#include "model/repository/Level.h"
#include "ServiceBase.h"

#include <memory>
#include <vector>

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
    [[nodiscard]] bool move(game::repo::Level& level, game::EntityId id, game::Position to) const;
    /**
     * @brief Метод расчета пути до выбранной позиции
     * @param level Уровень, на котором происходит поиск пути
     * @param id ID существа
     * @param to Конечная позиция
     * @return std::vector<Position> путь (без стартовой клетки)
     */
    [[nodiscard]] std::vector<game::Position> find_path(const game::repo::Level& level,
                                                           game::EntityId id,
                                                           game::Position to) const;
};

}

#endif // MYGAMEPROJECT_MOVEMENTSERVICE_H
