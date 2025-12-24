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

namespace game::entity {

class Enemy : public Entity {
public:
    Enemy() = default;
    explicit Enemy(game::EntityId id, std::string name = {}) : Entity(id, std::move(name)) {}

    [[nodiscard]] components::HealthComponent* health_comp() const { return get_component<components::HealthComponent>(); }
    [[nodiscard]] components::AIComponent* ai_comp() const { return get_component<components::AIComponent>(); }
    [[nodiscard]] components::MoveComponent* move_comp() const { return get_component<components::MoveComponent>(); }
    [[nodiscard]] components::TimePointsComponent* time_points_comp() const {
        return get_component<components::TimePointsComponent>();
    }
};

class Wild final : public Enemy {
public:
    Wild() = default;
    explicit Wild(game::EntityId id, std::string name = {}) : Enemy(id, std::move(name)) {}
    [[nodiscard]] components::CombatComponent* combat_comp() const { return get_component<components::CombatComponent>(); }
    [[nodiscard]] components::VisionComponent* vision_comp() const { return get_component<components::VisionComponent>(); }
};

class Intelligent final : public Enemy {
public:
    Intelligent() = default;
    explicit Intelligent(game::EntityId id, std::string name = {}) : Enemy(id, std::move(name)) {}
    [[nodiscard]] components::CombatComponent* combat_comp() const { return get_component<components::CombatComponent>(); }
    [[nodiscard]] components::VisionComponent* vision_comp() const { return get_component<components::VisionComponent>(); }
    [[nodiscard]] components::WeaponComponent* weapon_comp() const { return get_component<components::WeaponComponent>(); }
};

class Forager final : public Enemy {
public:
    Forager() = default;
    explicit Forager(game::EntityId id, std::string name = {}) : Enemy(id, std::move(name)) {}
    [[nodiscard]] components::InventoryComponent* inventory_comp() const {
        return get_component<components::InventoryComponent>();
    }
    [[nodiscard]] components::VisionComponent* vision_comp() const { return get_component<components::VisionComponent>(); }
};

}

#endif //INC_3_ENEMY_H
