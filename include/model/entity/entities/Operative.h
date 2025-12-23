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

namespace game {

class Operative final : public Entity {
public:
    Operative() = default;
    explicit Operative(EntityId id, std::string name = {}) : Entity(id, std::move(name)) {}

    [[nodiscard]] InventoryComponent* inventory_comp() const { return get_component<InventoryComponent>(); }
    [[nodiscard]] HealthComponent* health_comp() const { return get_component<HealthComponent>(); }
    [[nodiscard]] WeaponComponent* weapon_comp() const { return get_component<WeaponComponent>(); }
    [[nodiscard]] TimePointsComponent* time_points_comp() const { return get_component<TimePointsComponent>(); }
    [[nodiscard]] VisionComponent* vision_comp() const { return get_component<VisionComponent>(); }
    [[nodiscard]] CombatComponent* combat_comp() const { return get_component<CombatComponent>(); }
    [[nodiscard]] MoveComponent* move_comp() const { return get_component<MoveComponent>(); }
};

}

#endif //INC_3_OPERATIVE_H