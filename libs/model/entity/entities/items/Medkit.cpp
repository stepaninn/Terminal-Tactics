#include "model/entity/entities/items/Item.h"
#include "model/service/UseContext.h"
#include "model/entity/components/HealthComponent.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/components/TimePointsComponent.h"

namespace game::entity::items {

bool Medkit::use(service::UseContext& ctx) noexcept {
    auto* hp = ctx.hp;
    auto* tp = ctx.tp;
    auto* inv = ctx.inv;

    if (!hp || !tp || !inv) return false;

    if (tp->get_current_points() < time_cost_) return false;

    if (inv->get_item(id_) != this) return false;
    auto removed = inv->remove_by_id(id_);
    if (!removed) return false;

    if (tp->reduce_points(time_cost_) != time_cost_) {
        inv->add(std::move(removed));
        return false;
    }
    hp->add_hp(heal_hp_);

    return true;
}

}