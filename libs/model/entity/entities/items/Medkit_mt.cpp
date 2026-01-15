#include "model/entity/entities/items/Item_mt.h"
#include "model/service/UseContext_mt.h"
#include "model/entity/components/HealthComponent_mt.h"
#include "model/entity/components/InventoryComponent_mt.h"
#include "model/entity/components/TimePointsComponent_mt.h"

namespace game::mt::entity::items {

bool Medkit::use(service::UseContext& ctx) noexcept {
    auto* hp = ctx.hp;
    auto* tp = ctx.tp;
    auto* inv = ctx.inv;

    if (!hp || !tp || !inv) return false;

    if (tp->get_current_points() < get_cost()) return false;

    auto item_id = get_id();
    auto current_item = inv->get_item(item_id);
    if (!current_item || current_item.get() != this) return false;
    auto removed = inv->remove_by_id(item_id);
    if (!removed) return false;

    int cost = get_cost();
    if (tp->reduce_points(cost) != cost) {
        inv->add(std::move(removed));
        return false;
    }
    hp->add_hp(get_heal());

    return true;
}

}
