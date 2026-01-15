#ifndef MYGAMEPROJECT_COMBATSERVICE_MT_H
#define MYGAMEPROJECT_COMBATSERVICE_MT_H

#include "model/repository/Level_mt.h"
#include "model/entity/components/CombatComponent_mt.h"
#include "model/entity/components/InventoryComponent_mt.h"
#include "model/entity/entities/items/Weapon_mt.h"
#include "ServiceBase_mt.h"

#include <memory>
#include <random>

namespace game::mt::service {

/// @brief Сервис боя
class CombatService : public ServiceBase {
public:
    explicit CombatService(std::shared_ptr<events::EventBus> bus = nullptr)
        : ServiceBase(std::move(bus)), rng_(std::random_device{}()) {}
    explicit CombatService(size_t seed, std::shared_ptr<events::EventBus> bus = nullptr)
        : ServiceBase(std::move(bus)), rng_(seed) {}

    /**
     * @brief Метод проверки возможности атаки
     * @param attacker Атакующее существо
     * @return bool true, если существо может атаковать, false иначе
     */
    [[nodiscard]] static bool can_shoot(const game::mt::entity::Entity& attacker);

    /**
     * @brief Метод атаки
     * @param level Уровень, на котором происходит попытка атаки
     * @param attacker_id ID атакующего существа
     * @param target_id ID атакуемого существа
     * @return bool true, если выстрел успешен
     */
    [[nodiscard]] bool try_shoot(game::mt::repo::Level& level,
                                 game::mt::EntityId attacker_id,
                                 game::mt::EntityId target_id);

    /**
     * @brief Метод атаки
     * @param level Уровень, на котором происходит попытка атаки
     * @param attacker_id ID атакующего существа
     * @param pos Атакуемая позиция
     * @return bool true, если выстрел успешен
     */
    [[nodiscard]] bool try_shoot(game::mt::repo::Level& level,
                                 game::mt::EntityId attacker_id,
                                 game::mt::Position pos);

    /**
     * @brief Метод ближней атаки
     * @param level Уровень, на котором происходит атака
     * @param attacker_id ID атакующего существа
     * @param target_id ID атакуемого существа
     * @return bool true, если атака успешна
     */
    [[nodiscard]] bool melee_attack(game::mt::repo::Level& level,
                                    game::mt::EntityId attacker_id,
                                    game::mt::EntityId target_id);

    /**
     * @brief Метод перезарядки оружия
     * @param level Уровень, на котором происходит перезарядка
     * @param user_id ID существа, которое перезаряжает оружие
     * @return bool true, если перезарядка успешна
     */
    [[nodiscard]] static bool reload_weapon(game::mt::repo::Level& level,
                                     game::mt::EntityId user_id);
private:
    [[nodiscard]] int roll_damage(const game::mt::entity::items::Weapon& w) { return w.roll_damage(rng_); }
    // здесь настраивается точность (для баланса)
    [[nodiscard]] bool roll_hit(const game::mt::entity::components::CombatComponent& combat,
                                const game::mt::entity::items::Weapon& weapon,
                                int distance);

    std::mt19937 rng_;
};

}

#endif // MYGAMEPROJECT_COMBATSERVICE_MT_H
