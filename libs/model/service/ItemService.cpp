#include "model/service/ItemService.h"
#include "model/entity/components/HealthComponent.h"
#include "model/entity/components/TimePointsComponent.h"
#include "model/entity/components/WeaponComponent.h"

#include <memory>
#include <utility>

#include "model/service/UseContext.h"

namespace game::service {

bool ItemService::use_item(game::repo::Level& level,
        game::EntityId user_id, game::EntityId target_id, game::ItemId item_id) {
    auto* user = level.get_entity(user_id);
    auto* target = level.get_entity(target_id);
    if (!user || !target) return false;

    auto* inv = user->get_component<game::entity::components::InventoryComponent>();
    if (!inv) return false;

    game::entity::items::Item* item = inv->get_item(item_id);
    if (!item) return false;

    auto* hp = target->get_component<game::entity::components::HealthComponent>();
    auto* tp = user->get_component<game::entity::components::TimePointsComponent>();
    auto* wp = target->get_component<game::entity::components::WeaponComponent>();

    game::service::UseContext ctx{user_id, target_id, hp, tp, wp, inv};
    if (!item->use(ctx)) return false;

    if (auto* eb = bus()) {
        auto ev = std::make_shared<events::ItemUsedEvent>();
        ev->user_id = user_id;
        ev->item_id = item_id;
        ev->target_id = target_id;
        eb->publish(std::move(ev));
    }

    return true;
}

}
