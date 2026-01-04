#ifndef INC_3_TMP_SERVICE_ITEMSERVICE_H
#define INC_3_TMP_SERVICE_ITEMSERVICE_H

#include "model/repository/Level.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/entities/items/Item.h"
#include "ServiceBase.h"

#include <memory>

namespace game::service {

class ItemService : public ServiceBase {
public:
    explicit ItemService(std::shared_ptr<events::EventBus> bus = nullptr)  : ServiceBase(std::move(bus)) {}

    [[nodiscard]] bool use_item(game::repo::Level& level, game::EntityId user_id, game::ItemId item_id);

private:
    bool use_medkit(game::entity::Entity& user, game::entity::components::InventoryComponent& inv,
                    const game::entity::items::Medkit& kit);

    bool use_ammobag(game::entity::Entity& user, game::entity::components::InventoryComponent& inv,
                     const game::entity::items::AmmoBag& bag);

    void publish_used(game::EntityId user_id, game::ItemId item_id);
};

}

#endif // INC_3_TMP_SERVICE_ITEMSERVICE_H
