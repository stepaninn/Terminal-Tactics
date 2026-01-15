#ifndef MYGAMEPROJECT_MELEECOMPONENT_MT_H
#define MYGAMEPROJECT_MELEECOMPONENT_MT_H

#include "IComponent_mt.h"
#include <algorithm>
#include <mutex>

namespace game::mt::entity::components {

/// @brief Класс компонента ближней атаки
class MeleeComponent : public IComponent {
public:
    /**
     * @brief Метод получения урона ближней атаки
     * @return int урон ближней атаки
     */
    [[nodiscard]] virtual int get_damage() const noexcept = 0;
    /**
     * @brief Метод получения стоимости ближней атаки
     * @return int стоимость атаки (в очках времени)
     */
    [[nodiscard]] virtual int get_attack_cost() const noexcept = 0;

    /**
     * @brief Метод задания урона ближней атаки
     * @param dmg новый урон (минимум 1)
     */
    virtual void set_damage(int dmg) = 0;
    /**
     * @brief Метод задания стоимости ближней атаки
     * @param cost новая стоимость (минимум 1)
     */
    virtual void set_attack_cost(int cost) = 0;

    ~MeleeComponent() override = default;
};

class DefaultMeleeComp : public MeleeComponent {
public:
    DefaultMeleeComp() = default;
    DefaultMeleeComp(int dmg, int cost)
        : damage_(std::max(1, dmg)), attack_cost_(std::max(1, cost)) {}

    [[nodiscard]] int get_damage() const noexcept override {
        std::lock_guard<std::mutex> lock(mutex_);
        return damage_;
    }
    [[nodiscard]] int get_attack_cost() const noexcept override {
        std::lock_guard<std::mutex> lock(mutex_);
        return attack_cost_;
    }

    void set_damage(int dmg) override {
        std::lock_guard<std::mutex> lock(mutex_);
        damage_ = std::max(1, dmg);
    }
    void set_attack_cost(int cost) override {
        std::lock_guard<std::mutex> lock(mutex_);
        attack_cost_ = std::max(1, cost);
    }

private:
    mutable std::mutex mutex_;
    int damage_ = 1;
    int attack_cost_ = 1;
};

}

#endif // MYGAMEPROJECT_MELEECOMPONENT_MT_H
