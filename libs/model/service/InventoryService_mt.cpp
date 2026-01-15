#include "model/service/InventoryService_mt.h"

#include "model/entity/components/InventoryComponent_mt.h"
#include "model/repository/cells/ItemContainer_mt.h"
#include "model/service/events/Event_mt.h"
#include "model/service/EntityLock_mt.h"

#include <memory>
#include <utility>
#include <tbb/parallel_invoke.h>

namespace game::mt::service {

bool InventoryService::pick_item(game::mt::repo::Level& level,
                                 game::mt::EntityId entity_id,
                                 game::mt::ItemId item_id) const {
    auto lock = EntityLockPool::instance().lock_entity(entity_id);
    auto ent = level.get_entity(entity_id);
    if (!ent) return false;

    game::mt::entity::components::InventoryComponent* inv = nullptr;
    std::optional<game::mt::Position> pos;
    tbb::parallel_invoke(
        [&] { inv = ent->get_component<game::mt::entity::components::InventoryComponent>(); },
        [&] { pos = level.get_entity_position(entity_id); }
    );
    if (!inv) return false;
    if (inv->get_item(item_id)) return false;

    if (!pos) return false;

    auto* cell = level.get_cell(*pos);
    auto* container = dynamic_cast<game::mt::repo::cells::IItemContainer*>(cell);
    if (!container) return false;

    auto item = container->get_item(item_id);
    if (!item) return false;
    if (!inv->can_add(*item)) return false;

    auto picked = container->remove_by_id(item_id);
    if (!picked) return false;

    inv->add(picked);
    if (!inv->get_item(item_id)) {
        if (auto rejected = container->add(std::move(picked))) {
            inv->add(std::move(rejected));
        }
        return false;
    }

    if (auto* eb = bus()) {
        auto ev = std::make_shared<events::ItemPickedEvent>();
        ev->entity_id = entity_id;
        ev->item_id = item_id;
        eb->publish(std::move(ev));
    }

    return true;
}

bool InventoryService::drop_item(game::mt::repo::Level& level,
                                 game::mt::EntityId entity_id,
                                 game::mt::ItemId item_id) {
    auto lock = EntityLockPool::instance().lock_entity(entity_id);
    auto ent = level.get_entity(entity_id);
    if (!ent) return false;

    game::mt::entity::components::InventoryComponent* inv = nullptr;
    std::optional<game::mt::Position> pos;
    tbb::parallel_invoke(
        [&] { inv = ent->get_component<game::mt::entity::components::InventoryComponent>(); },
        [&] { pos = level.get_entity_position(entity_id); }
    );
    if (!inv) return false;

    if (!pos) return false;

    auto* cell = level.get_cell(*pos);
    auto* container = dynamic_cast<game::mt::repo::cells::IItemContainer*>(cell);
    if (!container || !container->can_place_items()) return false;

    auto removed = inv->remove_by_id(item_id);
    if (!removed) return false;

    if (auto rejected = container->add(std::move(removed))) {
        inv->add(std::move(rejected));
        return false;
    }

    if (auto* eb = bus()) {
        auto ev = std::make_shared<events::ItemDropEvent>();
        ev->entity_id = entity_id;
        ev->item_id = item_id;
        eb->publish(std::move(ev));
    }

    return true;
}

}
