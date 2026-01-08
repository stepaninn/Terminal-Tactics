#ifndef MYGAMEPROJECT_ITEMSERVICE_H
#define MYGAMEPROJECT_ITEMSERVICE_H

#include "model/repository/Level.h"
#include "model/entity/components/InventoryComponent.h"
#include "ServiceBase.h"

#include <memory>

namespace game::service {

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
    [[nodiscard]] bool use_item(game::repo::Level& level,
        game::EntityId user_id, game::EntityId target_id, game::ItemId item_id);
};

}

#endif // MYGAMEPROJECT_ITEMSERVICE_H
