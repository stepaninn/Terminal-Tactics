#include "model/service/CombatService_mt.h"

#include "model/entity/components/HealthComponent_mt.h"
#include "model/entity/components/InventoryComponent_mt.h"
#include "model/entity/components/MeleeComponent_mt.h"
#include "model/entity/components/TimePointsComponent_mt.h"
#include "model/entity/components/WeaponComponent_mt.h"
#include "model/entity/entities/items/Item_mt.h"
#include "model/repository/cells/DestructibleCell_mt.h"
#include "model/service/ItemService_mt.h"
#include "model/service/EntityLock_mt.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <random>
#include <memory>
#include <utility>
#include <tbb/parallel_for.h>
#include <tbb/parallel_invoke.h>

#include "model/service/VisionService_mt.h"
#include "model/service/events/Event_mt.h"

namespace game::mt::service {

bool CombatService::roll_hit(const game::mt::entity::components::CombatComponent& combat,
                             const game::mt::entity::items::Weapon& weapon,
                             int distance) {
    int range = weapon.get_range();
    if (range <= 0) return false;
    if (distance == 0) return true;

    double base_accuracy = combat.get_base_accuracy();

    double d = std::abs(distance - 1) / static_cast<double>(range);
    d = std::clamp(d, 0.0, 1.0);
    double coef = 1.0 - d * d;
    double capped = std::min(0.95, base_accuracy);
    double chance = std::clamp(capped * coef, 0.05, 1.0);

    std::uniform_real_distribution dist(0.0, 1.0);
    return dist(rng_) < chance;
}

bool CombatService::can_shoot(const game::mt::entity::Entity& attacker) {
    auto* wp_cmp = attacker.get_component<entity::components::WeaponComponent>();
    auto* tp = attacker.get_component<entity::components::TimePointsComponent>();
    if (!wp_cmp || !tp) return false;

    auto weapon = wp_cmp->get_weapon();
    if (!weapon) return false;
    if (weapon->get_current_ammo() == 0) return false;

    if (tp->get_current_points() < weapon->get_attack_cost()) return false;

    return true;
}

bool CombatService::try_shoot(game::mt::repo::Level& level,
                             game::mt::EntityId attacker_id,
                             game::mt::EntityId target_id) {
    auto locks = EntityLockPool::instance().lock_entities(attacker_id, target_id);
    std::shared_ptr<game::mt::entity::Entity> attacker;
    std::shared_ptr<game::mt::entity::Entity> target;
    tbb::parallel_invoke(
        [&] { attacker = level.get_entity(attacker_id); },
        [&] { target = level.get_entity(target_id); }
    );
    if (!attacker || !target) return false;

    auto* wp_cmp = static_cast<entity::components::WeaponComponent*>(nullptr);
    auto* cmb_cmp = static_cast<entity::components::CombatComponent*>(nullptr);
    auto* hp = static_cast<entity::components::HealthComponent*>(nullptr);
    auto* tp = static_cast<entity::components::TimePointsComponent*>(nullptr);
    tbb::parallel_invoke(
        [&] { wp_cmp = attacker->get_component<entity::components::WeaponComponent>(); },
        [&] { cmb_cmp = attacker->get_component<entity::components::CombatComponent>(); },
        [&] { tp = attacker->get_component<entity::components::TimePointsComponent>(); },
        [&] { hp = target->get_component<entity::components::HealthComponent>(); }
    );
    if (!wp_cmp || !cmb_cmp || !hp || !tp) return false;

    auto wp = wp_cmp->get_weapon();
    if (!wp) return false;

    std::optional<game::mt::Position> from_pos;
    std::optional<game::mt::Position> to_pos;
    tbb::parallel_invoke(
        [&] { from_pos = level.get_entity_position(attacker_id); },
        [&] { to_pos = level.get_entity_position(target_id); }
    );
    if (!from_pos || !to_pos) return false;
    auto from = *from_pos;
    auto to = *to_pos;

    if (!game::mt::service::VisionService::has_line_of_fire(level, from, to)) return false;

    int dx = to.x - from.x;
    int dy = to.y - from.y;
    int dist = std::max(std::abs(dx), std::abs(dy));

    if (!can_shoot(*attacker)) return false;

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
        return true;
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

bool CombatService::try_shoot(game::mt::repo::Level& level,
                              game::mt::EntityId attacker_id,
                              game::mt::Position pos) {
    if (!level.in_bounds(pos)) return false;
    auto attacker = level.get_entity(attacker_id);
    if (!attacker) return false;

    auto target = level.get_entity_at(pos);
    if (target) {
        return try_shoot(level, attacker_id, target->get_id());
    }

    auto* cell = level.get_cell(pos);
    auto* destructible = dynamic_cast<game::mt::repo::cells::IDestructibleCell*>(cell);
    if (!destructible || !destructible->can_be_shot()) return false;

    auto* wp_cmp = static_cast<entity::components::WeaponComponent*>(nullptr);
    auto* cmb_cmp = static_cast<entity::components::CombatComponent*>(nullptr);
    auto* tp = static_cast<entity::components::TimePointsComponent*>(nullptr);
    tbb::parallel_invoke(
        [&] { wp_cmp = attacker->get_component<entity::components::WeaponComponent>(); },
        [&] { cmb_cmp = attacker->get_component<entity::components::CombatComponent>(); },
        [&] { tp = attacker->get_component<entity::components::TimePointsComponent>(); }
    );
    if (!wp_cmp || !cmb_cmp || !tp) return false;

    auto wp = wp_cmp->get_weapon();
    if (!wp) return false;

    auto from_pos = level.get_entity_position(attacker_id);
    if (!from_pos) return false;
    auto from = *from_pos;

    if (!game::mt::service::VisionService::has_line_of_fire(level, from, pos)) return false;

    int dx = pos.x - from.x;
    int dy = pos.y - from.y;
    int dist = std::max(std::abs(dx), std::abs(dy));

    if (!can_shoot(*attacker)) return false;

    int cost = wp->get_attack_cost();
    if (tp->reduce_points(cost) != cost) return false;

    if (wp->reduce_ammo(1) != 1) return false;

    if (!roll_hit(*cmb_cmp, *wp, dist)) {
        if (auto* eb = bus()) {
            auto ev = std::make_shared<events::ShotMissedAtCellEvent>();
            ev->attacker_id = attacker_id;
            ev->pos = pos;
            eb->publish(std::move(ev));
        }
        return true;
    }

    bool destroyed = level.try_shoot(pos);
    if (destroyed) {
        if (auto* eb = bus()) {
            auto ev = std::make_shared<events::WallBrokenEvent>();
            ev->pos = pos;
            eb->publish(std::move(ev));
        }
    }
    return destroyed;
}

bool CombatService::melee_attack(game::mt::repo::Level& level,
                                 game::mt::EntityId attacker_id,
                                 game::mt::EntityId target_id) {
    auto locks = EntityLockPool::instance().lock_entities(attacker_id, target_id);
    std::shared_ptr<game::mt::entity::Entity> attacker;
    std::shared_ptr<game::mt::entity::Entity> target;
    tbb::parallel_invoke(
        [&] { attacker = level.get_entity(attacker_id); },
        [&] { target = level.get_entity(target_id); }
    );
    if (!attacker || !target) return false;

    std::optional<game::mt::Position> from_pos;
    std::optional<game::mt::Position> to_pos;
    tbb::parallel_invoke(
        [&] { from_pos = level.get_entity_position(attacker_id); },
        [&] { to_pos = level.get_entity_position(target_id); }
    );
    if (!from_pos || !to_pos) return false;

    int dx = std::abs(to_pos->x - from_pos->x);
    int dy = std::abs(to_pos->y - from_pos->y);
    if (std::max(dx, dy) > 1) return false;

    auto* melee = static_cast<entity::components::MeleeComponent*>(nullptr);
    auto* tp = static_cast<entity::components::TimePointsComponent*>(nullptr);
    auto* hp = static_cast<entity::components::HealthComponent*>(nullptr);
    tbb::parallel_invoke(
        [&] { melee = attacker->get_component<entity::components::MeleeComponent>(); },
        [&] { tp = attacker->get_component<entity::components::TimePointsComponent>(); },
        [&] { hp = target->get_component<entity::components::HealthComponent>(); }
    );
    if (!melee || !tp || !hp) return false;

    int cost = melee->get_attack_cost();
    if (tp->get_current_points() < cost) return false;
    if (tp->reduce_points(cost) != cost) return false;

    int dealt = hp->reduce_hp(melee->get_damage());
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

bool CombatService::reload_weapon(game::mt::repo::Level& level,
                                  game::mt::EntityId user_id) {
    auto lock = EntityLockPool::instance().lock_entity(user_id);
    auto user = level.get_entity(user_id);
    if (!user) return false;

    auto* wp_cmp = user->get_component<entity::components::WeaponComponent>();
    auto* inv = user->get_component<entity::components::InventoryComponent>();
    if (!wp_cmp || !inv) return false;

    auto weapon = wp_cmp->get_weapon();
    if (!weapon) return false;

    auto items = inv->get_items();
    if (items.empty()) return false;

    std::atomic<game::mt::ItemId> ammo_id{0};
    std::atomic<bool> found{false};
    tbb::parallel_for(size_t{0}, items.size(), [&](size_t i) {
        if (found.load()) return;
        const auto& item = items[i];
        auto* bag = item ? dynamic_cast<const game::mt::entity::items::AmmoBag*>(item.get()) : nullptr;
        if (!bag) return;
        if (bag->get_ammo_type() != weapon->get_ammo_type()) return;
        if (bag->get_current_ammo() <= 0) return;
        ammo_id.store(bag->get_id());
        found.store(true);
    });

    if (!found.load()) return false;

    ItemService items;
    return items.use_item(level, user_id, user_id, ammo_id.load());
}


}
