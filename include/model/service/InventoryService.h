#ifndef MYGAMEPROJECT_INVENTORYSERVICE_H
#define MYGAMEPROJECT_INVENTORYSERVICE_H

#include "model/repository/Level.h"
#include "ServiceBase.h"

#include <memory>

namespace game::service {

/// @brief Сервис инвентаря
class InventoryService : public ServiceBase {
public:
    explicit InventoryService(std::shared_ptr<events::EventBus> bus = nullptr) : ServiceBase(std::move(bus)) {}

    /**
     * @brief Метод подбирания предмета с клетки
     * @param level Уровень, на котором находится предмет
     * @param entity_id ID существа, которое подбирает предмет
     * @param item_id ID подбираемого предмета
     * @return bool true, если предмет успешно подобран
     */
    [[nodiscard]] bool pick_item(game::repo::Level& level,
                                 game::EntityId entity_id,
                                 game::ItemId item_id) const;

    /**
     * @brief Метод выбрасывания предмета из инвентаря на клетку
     * @param level Уровень, на котором находится существо
     * @param entity_id ID существа, которое выбрасывает предмет
     * @param item_id ID выбрасываемого предмета
     * @return bool true, если предмет успешно выброшен
     */
    [[nodiscard]] bool drop_item(game::repo::Level& level,
                                 game::EntityId entity_id,
                                 game::ItemId item_id);
};

}

#endif // MYGAMEPROJECT_INVENTORYSERVICE_H
