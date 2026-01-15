#ifndef MYGAMEPROJECT_INVENTORYSERVICE_MT_H
#define MYGAMEPROJECT_INVENTORYSERVICE_MT_H

#include "model/repository/Level_mt.h"
#include "ServiceBase_mt.h"

#include <memory>

namespace game::mt::service {

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
    [[nodiscard]] bool pick_item(game::mt::repo::Level& level,
                                 game::mt::EntityId entity_id,
                                 game::mt::ItemId item_id) const;

    /**
     * @brief Метод выбрасывания предмета из инвентаря на клетку
     * @param level Уровень, на котором находится существо
     * @param entity_id ID существа, которое выбрасывает предмет
     * @param item_id ID выбрасываемого предмета
     * @return bool true, если предмет успешно выброшен
     */
    [[nodiscard]] bool drop_item(game::mt::repo::Level& level,
                                 game::mt::EntityId entity_id,
                                 game::mt::ItemId item_id);
};

}

#endif // MYGAMEPROJECT_INVENTORYSERVICE_MT_H
