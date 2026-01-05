#ifndef INC_3_TMP_SERVICE_COMBATSERVICE_H
#define INC_3_TMP_SERVICE_COMBATSERVICE_H

#include "model/repository/Level.h"
#include "model/entity/components/CombatComponent.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/entities/items/Weapon.h"
#include "ServiceBase.h"

#include <memory>
#include <random>

namespace game::service {

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
    [[nodiscard]] static bool can_shoot(const game::entity::Entity& attacker);

    /**
     * @brief Метод атаки
     * @param level Уровень, на котором происходит попытка атаки
     * @param attacker_id ID атакующего существа
     * @param target_id ID атакуемого существа
     * @return bool true, если выстрел успешен
     */
    [[nodiscard]] bool try_shoot(game::repo::Level& level,
                                 game::EntityId attacker_id,
                                 game::EntityId target_id);

private:
    [[nodiscard]] int roll_damage(const game::entity::items::Weapon& w) { return w.roll_damage(rng_); }
    // здесь настраивается точность (для баланса)
    [[nodiscard]] bool roll_hit(const game::entity::components::CombatComponent& combat,
                                const game::entity::items::Weapon& weapon,
                                int distance);

    std::mt19937 rng_;
};

}

#endif // INC_3_TMP_SERVICE_COMBATSERVICE_H
