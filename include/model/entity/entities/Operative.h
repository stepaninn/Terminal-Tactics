#ifndef INC_3_OPERATIVE_H
#define INC_3_OPERATIVE_H

#include "Entity.h"

namespace game {

class Operative : public Entity {
public:
    Operative() = default;
    explicit Operative(id_t id, std::string name = {}) : Entity(id, std::move(name)) {}

    [[nodiscard]] std::shared_ptr<InventoryComponent> inventory_comp() const { return get_component<InventoryComponent>(); }
    [[nodiscard]] std::shared_ptr<HealthComponent> health_comp() const { return get_component<HealthComponent>(); }
    [[nodiscard]] std::shared_ptr<WeaponComponent> weapon_comp() const { return get_component<WeaponComponent>(); }
    [[nodiscard]] std::shared_ptr<TimePointsComponent> time_points_comp() const { return get_component<TimePointsComponent>(); }
    [[nodiscard]] std::shared_ptr<VisionComponent> vision_comp() const { return get_component<VisionComponent>(); }
    [[nodiscard]] std::shared_ptr<CombatComponent> combat_comp() const { return get_component<CombatComponent>(); }
    [[nodiscard]] std::shared_ptr<MoveComponent> move_comp() const { return get_component<MoveComponent>(); }
};

}

#endif //INC_3_OPERATIVE_H