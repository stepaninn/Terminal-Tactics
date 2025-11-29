#ifndef INC_3_ENEMY_H
#define INC_3_ENEMY_H

#include "Entity.h"

namespace game {

class Enemy : public Entity {
public:
    using Ptr = std::shared_ptr<Enemy>;
    Enemy() = default;
    explicit Enemy(id_t id, std::string name = {}) : Entity(id, std::move(name)) {}

    std::shared_ptr<HealthComponent> health_comp() const { return get_component<HealthComponent>(); }
    std::shared_ptr<AIComponent> ai_comp() const { return get_component<AIComponent>(); }
    std::shared_ptr<MoveComponent> move_comp() const { return get_component<MoveComponent>(); }
    std::shared_ptr<TimePointsComponent> time_points_comp() const { return get_component<TimePointsComponent>(); }
};

class Wild : public Enemy {
public:
    using Ptr = std::shared_ptr<Wild>;
    Wild() = default;
    std::shared_ptr<CombatComponent> combat_comp() const { return get_component<CombatComponent>(); }
    std::shared_ptr<VisionComponent> vision_comp() const { return get_component<VisionComponent>(); }
};

class Intelligent : public Enemy {
public:
    using Ptr = std::shared_ptr<Intelligent>;
    Intelligent() = default;
    std::shared_ptr<CombatComponent> combat_comp() const { return get_component<CombatComponent>(); }
    std::shared_ptr<VisionComponent> vision_comp() const { return get_component<VisionComponent>(); }
    std::shared_ptr<WeaponComponent> weapon_comp() const { return get_component<WeaponComponent>(); }
};

class Forager : public Enemy {
public:
    using Ptr = std::shared_ptr<Forager>;
    Forager() = default;
    std::shared_ptr<InventoryComponent> inventory_comp() const { return get_component<InventoryComponent>(); }
    std::shared_ptr<VisionComponent> vision_comp() const { return get_component<VisionComponent>(); }
};

}

#endif //INC_3_ENEMY_H