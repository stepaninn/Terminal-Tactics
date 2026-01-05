#include "model/entity/entities/items/Item.h"
#include "model/service/UseContext.h"
#include "model/entity/components/WeaponComponent.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/components/TimePointsComponent.h"

namespace game::entity::items {

int AmmoBag::add_ammo(int ammo) {
    if (ammo <= 0) return 0;
    int added = std::min(max_ammo_ - current_ammo_, ammo);
    current_ammo_ += added;
    return added;
}

int AmmoBag::reduce_ammo(int ammo) {
    if (ammo <= 0) return 0;
    int removed = std::min(current_ammo_, ammo);
    current_ammo_ -= removed;
    return removed;
}

bool AmmoBag::use(service::UseContext& ctx) noexcept {
    if (ctx.user != ctx.target) return false;

    auto* wp_cmp = ctx.wp;
    auto* tp = ctx.tp;
    auto* inv = ctx.inv;

    if (!wp_cmp || !tp || !inv) return false;

    auto* wp = wp_cmp->get_weapon();
    if (!wp) return false;

    if (tp->get_current_points() < wp->get_reload_cost()) return false;

    if (inv->get_item(id_) != this) return false;
    auto removed = inv->remove_by_id(id_);
    if (!removed) return false;

    int needed = wp->get_max_ammo() - wp->get_current_ammo();
    if (needed <= 0) {
        inv->add(std::move(removed));
        return false;
    }

    int gotten_ammo = this->reduce_ammo(needed);
    if (gotten_ammo <= 0) {
        inv->add(std::move(removed));
        return false;
    }

    if (tp->reduce_points(wp->get_reload_cost()) != wp->get_reload_cost()) {
        inv->add(std::move(removed));
        (void)this->add_ammo(gotten_ammo);
        return false;
    }

    (void)wp->add_ammo(gotten_ammo);

    if (this->get_current_ammo() > 0) {
        inv->add(std::move(removed));
    }

    return true;
}

}
