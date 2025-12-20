#ifndef INC_3_ENEMY_H
#define INC_3_ENEMY_H

#include "Entity.h"
#include "../components/HealthComponent.h"
#include "../components/AIComponent.h"
#include "../components/MoveComponent.h"
#include "../components/TimePointsComponent.h"
#include "../components/CombatComponent.h"
#include "../components/VisionComponent.h"
#include "../components/WeaponComponent.h"
#include "../components/InventoryComponent.h"

namespace game {

class Enemy : public Entity {
public:
    explicit Enemy(id_t id, std::string name = {}) : Entity(id, std::move(name)) {}

    [[nodiscard]] HealthComponent* health_comp() const { return get_component<HealthComponent>(); }
    [[nodiscard]] AIComponent* ai_comp() const { return get_component<AIComponent>(); }
    [[nodiscard]] MoveComponent* move_comp() const { return get_component<MoveComponent>(); }
    [[nodiscard]] TimePointsComponent* time_points_comp() const { return get_component<TimePointsComponent>(); }
};

class Wild final : public Enemy {
public:
    explicit Wild(id_t id, std::string name = {}) : Enemy(id, std::move(name)) {}
    [[nodiscard]] CombatComponent* combat_comp() const { return get_component<CombatComponent>(); }
    [[nodiscard]] VisionComponent* vision_comp() const { return get_component<VisionComponent>(); }
};

class Intelligent final : public Enemy {
public:
    explicit Intelligent(id_t id, std::string name = {}) : Enemy(id, std::move(name)) {}
    [[nodiscard]] CombatComponent* combat_comp() const { return get_component<CombatComponent>(); }
    [[nodiscard]] VisionComponent* vision_comp() const { return get_component<VisionComponent>(); }
    [[nodiscard]] WeaponComponent* weapon_comp() const { return get_component<WeaponComponent>(); }
};

class Forager final : public Enemy {
public:
    explicit Forager(id_t id, std::string name = {}) : Enemy(id, std::move(name)) {}
    [[nodiscard]] InventoryComponent* inventory_comp() const { return get_component<InventoryComponent>(); }
    [[nodiscard]] VisionComponent* vision_comp() const { return get_component<VisionComponent>(); }
};

}

#endif //INC_3_ENEMY_H