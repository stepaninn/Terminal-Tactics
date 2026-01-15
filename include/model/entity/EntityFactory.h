#ifndef MYGAMEPROJECT_ENTITYFACTORY_H
#define MYGAMEPROJECT_ENTITYFACTORY_H

#include "model/entity/entities/Enemy.h"
#include "model/entity/entities/Operative.h"
#include "model/entity/components/AIComponent.h"
#include "model/entity/components/CombatComponent.h"
#include "model/entity/components/HealthComponent.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/components/MeleeComponent.h"
#include "model/entity/components/MoveComponent.h"
#include "model/entity/components/TimePointsComponent.h"
#include "model/entity/components/VisionComponent.h"
#include "model/entity/components/WeaponComponent.h"
#include "model/entity/entities/items/Weapon.h"

#include <memory>
#include <string>
#include <utility>

namespace game::entity::factory {

struct OperativeConfig {
    int hp_current = 12;
    int hp_max = 12;
    int inventory_capacity = 6;
    int inventory_max_weight = 30;
    int move_step_cost = 1;
    int time_points_current = 100;
    int time_points_max = 100;
    double base_accuracy = 0.85;
    int melee_damage = 2;
    int melee_attack_cost = 1;
    int vision_radius = 9;
};

struct EnemyConfig {
    int hp_current = 9;
    int hp_max = 9;
    int move_step_cost = 1;
    int time_points_current = 8;
    int time_points_max = 8;
    double base_accuracy = 0.75;
    int melee_damage = 3;
    int melee_attack_cost = 1;
    int inventory_capacity = 4;
    int inventory_max_weight = 20;
    int vision_radius = 7;
};

class EntityFactory final {
public:
    static std::unique_ptr<entity::Operative> create_operative(game::EntityId id,
                                                               std::string name,
                                                               std::unique_ptr<entity::items::Weapon> weapon,
                                                               const OperativeConfig& cfg = {},
                                                               game::TeamId team_id = 0) {
        auto ent = std::make_unique<entity::Operative>(id, std::move(name));
        ent->set_team_id(team_id);
        ent->add_component<components::HealthComponent, components::DefaultHealthComp>(
            cfg.hp_current, cfg.hp_max);
        ent->add_component<components::InventoryComponent, components::DefaultInventoryComp>(
            cfg.inventory_capacity, cfg.inventory_max_weight);
        ent->add_component<components::MoveComponent, components::DefaultMoveComp>(cfg.move_step_cost);
        ent->add_component<components::TimePointsComponent, components::DefaultTimePointsComp>(
            cfg.time_points_current, cfg.time_points_max);
        ent->add_component<components::CombatComponent, components::DefaultCombatComp>(cfg.base_accuracy);
        ent->add_component<components::MeleeComponent, components::DefaultMeleeComp>(
            cfg.melee_damage, cfg.melee_attack_cost);
        ent->add_component<components::WeaponComponent, components::DefaultWeaponComp>(std::move(weapon));
        ent->add_component<components::VisionComponent, components::DefaultVisionComp>(cfg.vision_radius);
        return ent;
    }

    static std::unique_ptr<entity::Wild> create_wild(game::EntityId id,
                                                     std::string name,
                                                     std::unique_ptr<entity::items::Weapon> weapon,
                                                     const EnemyConfig& cfg = {},
                                                     game::TeamId team_id = 1) {
        return create_enemy<entity::Wild>(id, std::move(name), std::move(weapon),
                                          components::AIBehavior::WILD, cfg, team_id);
    }

    static std::unique_ptr<entity::Intelligent> create_intelligent(game::EntityId id,
                                                                   std::string name,
                                                                   std::unique_ptr<entity::items::Weapon> weapon,
                                                                   const EnemyConfig& cfg = {},
                                                                   game::TeamId team_id = 1) {
        return create_enemy<entity::Intelligent>(id, std::move(name), std::move(weapon),
                                                 components::AIBehavior::INTELLIGENT, cfg, team_id);
    }

    static std::unique_ptr<entity::Forager> create_forager(game::EntityId id,
                                                           std::string name,
                                                           std::unique_ptr<entity::items::Weapon> weapon,
                                                           const EnemyConfig& cfg = {},
                                                           game::TeamId team_id = 1) {
        return create_enemy<entity::Forager>(id, std::move(name), std::move(weapon),
                                             components::AIBehavior::FORAGER, cfg, team_id);
    }

private:
    template <typename TEnemy>
    static std::unique_ptr<TEnemy> create_enemy(game::EntityId id,
                                                std::string name,
                                                std::unique_ptr<entity::items::Weapon> weapon,
                                                components::AIBehavior behavior,
                                                const EnemyConfig& cfg,
                                                game::TeamId team_id) {
        auto ent = std::make_unique<TEnemy>(id, std::move(name));
        ent->set_team_id(team_id);
        ent->template add_component<components::AIComponent, components::DefaultAIComp>(behavior);
        ent->template add_component<components::HealthComponent, components::DefaultHealthComp>(
            cfg.hp_current, cfg.hp_max);
        ent->template add_component<components::MoveComponent, components::DefaultMoveComp>(cfg.move_step_cost);
        ent->template add_component<components::TimePointsComponent, components::DefaultTimePointsComp>(
            cfg.time_points_current, cfg.time_points_max);
        ent->template add_component<components::CombatComponent, components::DefaultCombatComp>(cfg.base_accuracy);
        ent->template add_component<components::MeleeComponent, components::DefaultMeleeComp>(
            cfg.melee_damage, cfg.melee_attack_cost);
        ent->template add_component<components::InventoryComponent, components::DefaultInventoryComp>(
            cfg.inventory_capacity, cfg.inventory_max_weight);
        ent->template add_component<components::WeaponComponent, components::DefaultWeaponComp>(std::move(weapon));
        ent->template add_component<components::VisionComponent, components::DefaultVisionComp>(cfg.vision_radius);
        return ent;
    }
};

}

#endif // MYGAMEPROJECT_ENTITYFACTORY_H
