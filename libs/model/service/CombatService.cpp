#include "model/service/CombatService.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <utility>

#include "model/service/events/Event.h"

namespace game::service {


int CombatService::roll_damage(const game::entity::items::Weapon& weapon) const {
    auto dmg = weapon.get_dmg();
    int max_dmg = dmg.max_dmg;
    int min_dmg = dmg.min_dmg;
    if (max_dmg < min_dmg) std::swap(max_dmg, min_dmg);

    int real_dmg = min_dmg + std::rand() % (max_dmg - min_dmg + 1);
    return real_dmg;
}

bool CombatService::roll_hit(const game::entity::components::CombatComponent& combat,
                             const game::entity::items::Weapon& weapon,
                             int distance) const {
    // sqrt(-dist + 11) * 0.316
    if (distance > 10 || distance > weapon.get_range()) return false;
    double chance = std::sqrt(-distance + 11) * 0.316 * combat.get_base_accuracy();
    if (chance <= 0.0) return false;
    if (chance > 100.0) chance = 100.0;
    return (std::rand() % 101) < chance;
}

bool CombatService::can_shoot(const game::entity::Entity& attacker,
                             const game::entity::items::Weapon& weapon) const {
    if (weapon.get_current_ammo() == 0) return false;
    auto* tp = attacker.get_component<entity::components::TimePointsComponent>();
    if (!tp || tp->get_current_points() < weapon.get_attack_cost()) return false;
    return true;
}

bool CombatService::try_shoot(game::repo::Level& level,
                             game::EntityId attacker_id,
                             game::EntityId target_id) {
    auto attacker = level.get_entity(attacker_id);
    auto target = level.get_entity(target_id);
    if (!attacker || !target) return false;

    auto* wp_cmp = attacker->get_component<entity::components::WeaponComponent>();
    auto* cmb_cmp = attacker->get_component<entity::components::CombatComponent>();
    auto* from_cmp = attacker->get_component<entity::components::PositionComponent>();
    auto* to_cmp = target->get_component<entity::components::PositionComponent>();
    auto* hp = target->get_component<entity::components::HealthComponent>();
    auto* tp = attacker->get_component<entity::components::TimePointsComponent>();
    if (!wp_cmp || !cmb_cmp || !from_cmp || !to_cmp || !hp || !tp) return false;

    auto* wp = wp_cmp->get_weapon();
    if (!wp) return false;

    auto from = from_cmp->get_position();
    auto to = to_cmp->get_position();

    int dx = static_cast<int>(to.x) - static_cast<int>(from.x);
    int dy = static_cast<int>(to.y) - static_cast<int>(from.y);
    int dist = std::max(std::abs(dx), std::abs(dy));

    if (!can_shoot(*attacker, *wp)) return false;

    int cost = wp->get_attack_cost();
    if (tp->reduce_points(cost) != cost) return false;

    if (wp->reduce_ammo(1) != 1) return false;

    if (!roll_hit(*cmb_cmp, *wp, dist)) {
        if (auto* eb = bus()) {
            auto ev = std::make_shared<events::ShotMissedEvent>();
            ev->attacker_id = attacker_id;
            ev->target_id = target_id;
            eb->publish(std::move(ev));
        }
        return false;
    }

    int dmg = roll_damage(*wp);
    int dealt = hp->reduce_hp(dmg);
    if (auto* eb = bus()) {
        auto ev = std::make_shared<events::DamageEvent>();
        ev->attacker_id = attacker_id;
        ev->target_id = target_id;
        ev->amount = dealt;
        eb->publish(std::move(ev));
    }

    if (hp->get_current_hp() == 0) {
        if (auto* eb = bus()) {
            auto ev = std::make_shared<events::EntityDiedEvent>();
            ev->entity_id = target_id;
            ev->killer_id = attacker_id;
            eb->publish(std::move(ev));
        }
    }
    return true;
}

bool CombatService::reload_weapon(game::entity::Entity& user, game::ItemId ammo_bag_id) {
    auto* inv = user.get_component<entity::components::InventoryComponent>();
    auto* tp = user.get_component<entity::components::TimePointsComponent>();
    auto* wp_cmp = user.get_component<entity::components::WeaponComponent>();
    if (!inv || !tp || !wp_cmp) return false;

    auto* wp = wp_cmp->get_weapon();
    if (!wp) return false;

    if (tp->get_current_points() < wp->get_reload_cost()) return false;

    auto removed = inv->remove_by_id(ammo_bag_id);
    if (!removed) return false;

    auto* ammo_bag = dynamic_cast<entity::items::AmmoBag*>(removed.get());
    if (!ammo_bag || ammo_bag->get_ammo_type() != wp->get_ammo_type()) {
        inv->add(std::move(removed), ammo_bag_id);
        return false;
    }

    int needed = wp->get_max_ammo() - wp->get_current_ammo();
    if (needed <= 0) {
        inv->add(std::move(removed), ammo_bag_id);
        return false;
    }

    int gotten_ammo = ammo_bag->reduce_ammo(needed);
    if (gotten_ammo <= 0) {
        inv->add(std::move(removed), ammo_bag_id);
        return false;
    }

    (void)wp->add_ammo(gotten_ammo);
    if (tp->reduce_points(wp->get_reload_cost()) != wp->get_reload_cost()) return false;

    if (ammo_bag->get_current_ammo() > 0) {
        inv->add(std::move(removed), ammo_bag_id);
    }

    return true;
}

bool CombatService::apply_damage(game::repo::Level& level, game::EntityId attacker_id,
                                 game::EntityId target_id, int amount) {
    if (amount <= 0) return false;
    auto* target = level.get_entity(target_id);
    if (!target) return false;
    auto* hp = target->get_component<game::entity::components::HealthComponent>();
    if (!hp) return false;

    int dealt = hp->reduce_hp(amount);
    if (auto* eb = bus()) {
        auto ev = std::make_shared<events::DamageEvent>();
        ev->attacker_id = attacker_id;
        ev->target_id = target_id;
        ev->amount = dealt;
        eb->publish(std::move(ev));
    }

    if (hp->get_current_hp() == 0) {
        if (auto* eb = bus()) {
            auto ev = std::make_shared<events::EntityDiedEvent>();
            ev->entity_id = target_id;
            ev->killer_id = attacker_id;
            eb->publish(std::move(ev));
        }
    }
    return true;
}

}
