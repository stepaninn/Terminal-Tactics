#ifndef MYGAMEPROJECT_ENEMY_MT_H
#define MYGAMEPROJECT_ENEMY_MT_H

#include "Entity_mt.h"
#include "../components/HealthComponent_mt.h"
#include "../components/AIComponent_mt.h"
#include "../components/MoveComponent_mt.h"
#include "../components/TimePointsComponent_mt.h"
#include "../components/CombatComponent_mt.h"
#include "../components/VisionComponent_mt.h"
#include "../components/WeaponComponent_mt.h"
#include "../components/InventoryComponent_mt.h"

namespace game::mt::entity {

/// @brief Базовый класс противника
class Enemy : public Entity {
public:
    Enemy() = default;
    explicit Enemy(game::mt::EntityId id, std::string name = {}) : Entity(id, std::move(name)) {}

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
    explicit Wild(game::mt::EntityId id, std::string name = {}) : Enemy(id, std::move(name)) {}
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
    explicit Intelligent(game::mt::EntityId id, std::string name = {}) : Enemy(id, std::move(name)) {}
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
    explicit Forager(game::mt::EntityId id, std::string name = {}) : Enemy(id, std::move(name)) {}
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

#endif //MYGAMEPROJECT_ENEMY_MT_H
