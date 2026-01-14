#include "model/service/InventoryService.h"

#include "model/entity/components/InventoryComponent.h"
#include "model/repository/cells/ItemContainer.h"
#include "model/service/events/Event.h"

#include <memory>
#include <utility>

namespace game::service {

bool InventoryService::pick_item(game::repo::Level& level,
                                 game::EntityId entity_id,
                                 game::ItemId item_id) const {
    auto* ent = level.get_entity(entity_id);
    if (!ent) return false;

    auto* inv = ent->get_component<game::entity::components::InventoryComponent>();
    if (!inv) return false;
    if (inv->get_item(item_id)) return false;

    const auto* pos = level.get_entity_position(entity_id);
    if (!pos) return false;

    auto* cell = level.get_cell(*pos);
    auto* container = dynamic_cast<game::repo::cells::IItemContainer*>(cell);
    if (!container) return false;

    const auto* item = container->get_item(item_id);
    if (!item) return false;
    if (!inv->can_add(*item)) return false;

    auto picked = container->remove_by_id(item_id);
    if (!picked) return false;

    inv->add(std::move(picked));

    if (auto* eb = bus()) {
        auto ev = std::make_shared<events::ItemPickedEvent>();
        ev->entity_id = entity_id;
        ev->item_id = item_id;
        eb->publish(std::move(ev));
    }

    return true;
}

bool InventoryService::drop_item(game::repo::Level& level,
                                 game::EntityId entity_id,
                                 game::ItemId item_id) {
    auto* ent = level.get_entity(entity_id);
    if (!ent) return false;

    auto* inv = ent->get_component<game::entity::components::InventoryComponent>();
    if (!inv) return false;

    const auto* pos = level.get_entity_position(entity_id);
    if (!pos) return false;

    auto* cell = level.get_cell(*pos);
    auto* container = dynamic_cast<game::repo::cells::IItemContainer*>(cell);
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
