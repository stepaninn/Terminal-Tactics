#ifndef MYGAMEPROJECT_ITEMSERVICE_MT_H
#define MYGAMEPROJECT_ITEMSERVICE_MT_H

#include "model/repository/Level_mt.h"
#include "model/entity/components/InventoryComponent_mt.h"
#include "ServiceBase_mt.h"

#include <memory>

namespace game::mt::service {

/// @brief Сервис использования предметов
class ItemService : public ServiceBase {
public:
    explicit ItemService(std::shared_ptr<events::EventBus> bus = nullptr) : ServiceBase(std::move(bus)) {}

    /**
     * @brief Метод использования предмета
     * @param level Уровень, на котором используется предмет
     * @param user_id ID существа, которое использует предмет
     * @param target_id ID существа, на котором используется предмет
     * @param item_id ID используемого предмета
     * @return bool true, если использование успешно
     */
    [[nodiscard]] bool use_item(game::mt::repo::Level& level,
        game::mt::EntityId user_id, game::mt::EntityId target_id, game::mt::ItemId item_id);
};

}

#endif // MYGAMEPROJECT_ITEMSERVICE_MT_H
