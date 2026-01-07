#ifndef MYGAMEPROJECT_ENEMY_H
#define MYGAMEPROJECT_ENEMY_H

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

/// @brief Базовый класс противника
class Enemy : public Entity {
public:
    Enemy() = default;
    explicit Enemy(game::EntityId id, std::string name = {}) : Entity(id, std::move(name)) {}

    /**
     * @brief Метод получения компонента здоровья
     * @return HealthComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::HealthComponent* health_comp() const { return get_component<components::HealthComponent>(); }
    /**
     * @brief Метод получения компонента ИИ
     * @return AIComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::AIComponent* ai_comp() const { return get_component<components::AIComponent>(); }
    /**
     * @brief Метод получения компонента перемещения
     * @return MoveComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::MoveComponent* move_comp() const { return get_component<components::MoveComponent>(); }
    /**
     * @brief Метод получения компонента очков времени
     * @return TimePointsComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::TimePointsComponent* time_points_comp() const {
        return get_component<components::TimePointsComponent>();
    }
};

/// @brief Противник дикого типа
class Wild final : public Enemy {
public:
    Wild() = default;
    explicit Wild(game::EntityId id, std::string name = {}) : Enemy(id, std::move(name)) {}
    /**
     * @brief Метод получения боевого компонента
     * @return CombatComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::CombatComponent* combat_comp() const { return get_component<components::CombatComponent>(); }
    /**
     * @brief Метод получения компонента зрения
     * @return VisionComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::VisionComponent* vision_comp() const { return get_component<components::VisionComponent>(); }
};

/// @brief Противник разумного типа
class Intelligent final : public Enemy {
public:
    Intelligent() = default;
    explicit Intelligent(game::EntityId id, std::string name = {}) : Enemy(id, std::move(name)) {}
    /**
     * @brief Метод получения боевого компонента
     * @return CombatComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::CombatComponent* combat_comp() const { return get_component<components::CombatComponent>(); }
    /**
     * @brief Метод получения компонента зрения
     * @return VisionComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::VisionComponent* vision_comp() const { return get_component<components::VisionComponent>(); }
    /**
     * @brief Метод получения компонента оружия
     * @return WeaponComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::WeaponComponent* weapon_comp() const { return get_component<components::WeaponComponent>(); }
};

/// @brief Противник собирательского типа
class Forager final : public Enemy {
public:
    Forager() = default;
    explicit Forager(game::EntityId id, std::string name = {}) : Enemy(id, std::move(name)) {}
    /**
     * @brief Метод получения компонента инвентаря
     * @return InventoryComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::InventoryComponent* inventory_comp() const {
        return get_component<components::InventoryComponent>();
    }
    /**
     * @brief Метод получения компонента зрения
     * @return VisionComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::VisionComponent* vision_comp() const { return get_component<components::VisionComponent>(); }
};

}

#endif //MYGAMEPROJECT_ENEMY_H
