#ifndef INC_3_OPERATIVE_H
#define INC_3_OPERATIVE_H

#include "Entity.h"

namespace game {

class Operative : public Entity {
public:
    using Ptr = std::shared_ptr<Operative>;
    Operative() = default;
    explicit Operative(id_t id, std::string name = {}) : Entity(id, std::move(name)) {}

    std::shared_ptr<InventoryComponent> inventory_comp() const { return get_component<InventoryComponent>(); }
    std::shared_ptr<HealthComponent> health_comp() const { return get_component<HealthComponent>(); }
    std::shared_ptr<WeaponComponent> weapon_comp() const { return get_component<WeaponComponent>(); }
    std::shared_ptr<TimePointsComponent> time_points_comp() const { return get_component<TimePointsComponent>(); }
    std::shared_ptr<VisionComponent> vision_comp() const { return get_component<VisionComponent>(); }
    std::shared_ptr<CombatComponent> combat_comp() const { return get_component<CombatComponent>(); }
    std::shared_ptr<MoveComponent> move_comp() const { return get_component<MoveComponent>(); }
};

}

#endif //INC_3_OPERATIVE_H