#include "model/service/CombatService.h"
#include "model/entity/components/HealthComponent.h"

#include <memory>
#include <utility>

namespace game::service {

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
