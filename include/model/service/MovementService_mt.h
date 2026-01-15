#ifndef MYGAMEPROJECT_MOVEMENTSERVICE_MT_H
#define MYGAMEPROJECT_MOVEMENTSERVICE_MT_H

#include "model/repository/Level_mt.h"
#include "ServiceBase_mt.h"

#include <memory>
#include <vector>

namespace game::mt::service {

/// @brief Сервис передвижения
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
    [[nodiscard]] bool move(game::mt::repo::Level& level, game::mt::EntityId id, game::mt::Position to) const;
    /**
     * @brief Метод расчета пути до выбранной позиции
     * @param level Уровень, на котором происходит поиск пути
     * @param id ID существа
     * @param to Конечная позиция
     * @return std::vector<Position> путь (без стартовой клетки)
     */
    [[nodiscard]] static std::vector<game::mt::Position> find_path(const game::mt::repo::Level& level,
                                                           game::mt::EntityId id,
                                                           game::mt::Position to) ;
    /**
     * @brief Метод расчета пути до клетки перед целью
     * @param level Уровень, на котором происходит поиск пути
     * @param id ID существа
     * @param to Конечная позиция
     * @return std::vector<Position> путь (без стартовой и без конечной клетки)
     */
    [[nodiscard]] static std::vector<game::mt::Position> find_path_without_target(const game::mt::repo::Level& level,
                                                                       game::mt::EntityId id,
                                                                       game::mt::Position to) ;
};

}

#endif // MYGAMEPROJECT_MOVEMENTSERVICE_MT_H
