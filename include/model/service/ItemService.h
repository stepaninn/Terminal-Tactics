#ifndef INC_3_TMP_SERVICE_ITEMSERVICE_H
#define INC_3_TMP_SERVICE_ITEMSERVICE_H

#include "../repository/Level.h"
#include "../entity/components/InventoryComponent.h"
#include "../entity/components/HealthComponent.h"
#include "../entity/components/TimePointsComponent.h"
#include "../entity/components/WeaponComponent.h"
#include "../entity/entities/items/Item.h"
#include "../entity/entities/items/Weapon.h"
#include "events/Event.h"
#include "ServiceBase.h"

namespace game::service {

class ItemService : public ServiceBase {
public:
    explicit ItemService(std::shared_ptr<events::EventBus> bus = nullptr) : ServiceBase(std::move(bus)) {}

    [[nodiscard]] bool use_item(game::repo::Level& level, game::EntityId user_id, game::ItemId item_id) {
        auto* user = level.get_entity(user_id);
        if (!user) return false;

        auto* inv = user->get_component<game::entity::components::InventoryComponent>();
        if (!inv) return false;

        const game::entity::items::Item* item = inv->get_item(item_id);
        if (!item) return false;

        if (auto* kit = dynamic_cast<const game::entity::items::Medkit*>(item)) {
            return use_medkit(*user, *inv, *kit, item_id);
        }
        if (auto* bag = dynamic_cast<const game::entity::items::AmmoBag*>(item)) {
            return use_ammobag(*user, *inv, *bag, item_id);
        }
        return false;
    }

private:
    bool use_medkit(game::entity::Entity& user, game::entity::components::InventoryComponent& inv,
                    const game::entity::items::Medkit& kit, game::ItemId item_id) {
        auto* hp = user.get_component<game::entity::components::HealthComponent>();
        auto* tp = user.get_component<game::entity::components::TimePointsComponent>();
        if (!hp || !tp) return false;
        if (tp->get_current_points() < kit.get_cost()) return false;
        if (tp->reduce_points(kit.get_cost()) != kit.get_cost()) return false;

        auto removed = inv.remove_by_id(item_id);
        if (!removed) return false;

        hp->add_hp(kit.get_heal());
        publish_used(user.get_id(), item_id);
        return true;
    }

    bool use_ammobag(game::entity::Entity& user, game::entity::components::InventoryComponent& inv,
                     const game::entity::items::AmmoBag& bag, game::ItemId item_id) {
        auto* wc = user.get_component<game::entity::components::WeaponComponent>();
        if (!wc) return false;
        auto weapon = wc->get_weapon();
        if (!weapon || weapon->get_ammo_type() != bag.get_ammo_type()) return false;

        auto removed = inv.remove_by_id(item_id);
        if (!removed) return false;

        auto* bag_item = dynamic_cast<game::entity::items::AmmoBag*>(removed.get());
        if (!bag_item) return false;

        (void)weapon->add_ammo(bag_item->get_current_ammo());
        publish_used(user.get_id(), item_id);
        return true;
    }

    void publish_used(game::EntityId user_id, game::ItemId item_id) {
        if (auto* eb = bus()) {
            auto ev = std::make_shared<events::ItemUsedEvent>();
            ev->user_id = user_id;
            ev->item_id = item_id;
            ev->target_id = user_id;
            eb->publish(std::move(ev));
        }
    }
};

}

#endif // INC_3_TMP_SERVICE_ITEMSERVICE_H
