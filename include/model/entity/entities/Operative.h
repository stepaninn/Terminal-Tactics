#ifndef INC_3_OPERATIVE_H
#define INC_3_OPERATIVE_H

#include "Entity.h"
#include "../components/InventoryComponent.h"
#include "../components/HealthComponent.h"
#include "../components/WeaponComponent.h"
#include "../components/TimePointsComponent.h"
#include "../components/VisionComponent.h"
#include "../components/CombatComponent.h"
#include "../components/MoveComponent.h"

namespace game::entity {

class Operative final : public Entity {
public:
    Operative() = default;
    explicit Operative(game::EntityId id, std::string name = {}) : Entity(id, std::move(name)) {}

    [[nodiscard]] components::InventoryComponent* inventory_comp() const {
        return get_component<components::InventoryComponent>();
    }
    [[nodiscard]] components::HealthComponent* health_comp() const { return get_component<components::HealthComponent>(); }
    [[nodiscard]] components::WeaponComponent* weapon_comp() const { return get_component<components::WeaponComponent>(); }
    [[nodiscard]] components::TimePointsComponent* time_points_comp() const {
        return get_component<components::TimePointsComponent>();
    }
    [[nodiscard]] components::VisionComponent* vision_comp() const { return get_component<components::VisionComponent>(); }
    [[nodiscard]] components::CombatComponent* combat_comp() const { return get_component<components::CombatComponent>(); }
    [[nodiscard]] components::MoveComponent* move_comp() const { return get_component<components::MoveComponent>(); }
};

}

#endif //INC_3_OPERATIVE_H
