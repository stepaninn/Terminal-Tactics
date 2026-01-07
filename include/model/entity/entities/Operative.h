#ifndef MYGAMEPROJECT_OPERATIVE_H
#define MYGAMEPROJECT_OPERATIVE_H

#include "Entity.h"
#include "../components/InventoryComponent.h"
#include "../components/HealthComponent.h"
#include "../components/WeaponComponent.h"
#include "../components/TimePointsComponent.h"
#include "../components/VisionComponent.h"
#include "../components/CombatComponent.h"
#include "../components/MoveComponent.h"

namespace game::entity {

/// @brief Класс оперативника
class Operative final : public Entity {
public:
    Operative() = default;
    explicit Operative(game::EntityId id, std::string name = {}) : Entity(id, std::move(name)) {}

    /**
     * @brief Метод получения компонента инвентаря
     * @return InventoryComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::InventoryComponent* inventory_comp() const {
        return get_component<components::InventoryComponent>();
    }
    /**
     * @brief Метод получения компонента здоровья
     * @return HealthComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::HealthComponent* health_comp() const { return get_component<components::HealthComponent>(); }
    /**
     * @brief Метод получения компонента оружия
     * @return WeaponComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::WeaponComponent* weapon_comp() const { return get_component<components::WeaponComponent>(); }
    /**
     * @brief Метод получения компонента очков времени
     * @return TimePointsComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::TimePointsComponent* time_points_comp() const {
        return get_component<components::TimePointsComponent>();
    }
    /**
     * @brief Метод получения компонента зрения
     * @return VisionComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::VisionComponent* vision_comp() const { return get_component<components::VisionComponent>(); }
    /**
     * @brief Метод получения боевого компонента
     * @return CombatComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::CombatComponent* combat_comp() const { return get_component<components::CombatComponent>(); }
    /**
     * @brief Метод получения компонента перемещения
     * @return MoveComponent* указатель на компонент или nullptr
     */
    [[nodiscard]] components::MoveComponent* move_comp() const { return get_component<components::MoveComponent>(); }
};

}

#endif //MYGAMEPROJECT_OPERATIVE_H
