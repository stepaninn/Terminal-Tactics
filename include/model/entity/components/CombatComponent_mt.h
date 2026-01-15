#ifndef MYGAMEPROJECT_COMBATCOMPONENT_MT_H
#define MYGAMEPROJECT_COMBATCOMPONENT_MT_H

#include "IComponent_mt.h"
#include <algorithm>
#include <mutex>

namespace game::mt::entity::components {

/// @brief Класс боевого компонента
class CombatComponent : public IComponent {
public:
    /**
     * @brief Метод получения точности компонента
     * @return double точность для компонента (0-1)
     */
    [[nodiscard]] virtual double get_base_accuracy() const = 0;
    /**
     * @brief Операция задания точности
     * @param acc Точность (0-1)
     */
    virtual void set_base_accuracy(double acc) = 0;
    ~CombatComponent() override = default;
};

class DefaultCombatComp : public CombatComponent {
public:
    DefaultCombatComp() = default;
    explicit DefaultCombatComp(double acc) : accuracy_(std::clamp(acc, 0.0, 1.0)) {}
    [[nodiscard]] double get_base_accuracy() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return accuracy_;
    }
    void set_base_accuracy(double acc) override {
        std::lock_guard<std::mutex> lock(mutex_);
        accuracy_ = std::clamp(acc, 0.0, 1.0);
    }

protected:
    mutable std::mutex mutex_;
    double accuracy_ = 0;
};

}

#endif //MYGAMEPROJECT_COMBATCOMPONENT_MT_H
