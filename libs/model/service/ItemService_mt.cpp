#include "model/service/ItemService_mt.h"
#include "model/entity/components/HealthComponent_mt.h"
#include "model/entity/components/TimePointsComponent_mt.h"
#include "model/entity/components/WeaponComponent_mt.h"
#include "model/service/EntityLock_mt.h"

#include <memory>
#include <utility>

#include "model/service/UseContext_mt.h"
#include <tbb/parallel_invoke.h>

namespace game::mt::service {

bool ItemService::use_item(game::mt::repo::Level& level,
        game::mt::EntityId user_id, game::mt::EntityId target_id, game::mt::ItemId item_id) {
    auto locks = EntityLockPool::instance().lock_entities(user_id, target_id);
    auto user = level.get_entity(user_id);
    auto target = level.get_entity(target_id);
    if (!user || !target) return false;

    auto* inv = user->get_component<game::mt::entity::components::InventoryComponent>();
    if (!inv) return false;

    auto item = inv->get_item(item_id);
    if (!item) return false;

    game::mt::entity::components::HealthComponent* hp = nullptr;
    game::mt::entity::components::TimePointsComponent* tp = nullptr;
    game::mt::entity::components::WeaponComponent* wp = nullptr;
    tbb::parallel_invoke(
        [&] { hp = target->get_component<game::mt::entity::components::HealthComponent>(); },
        [&] { tp = user->get_component<game::mt::entity::components::TimePointsComponent>(); },
        [&] { wp = target->get_component<game::mt::entity::components::WeaponComponent>(); }
    );

    game::mt::service::UseContext ctx{user_id, target_id, hp, tp, wp, inv};
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
