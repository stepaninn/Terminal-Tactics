#ifndef INC_3_COMBATCOMPONENT_H
#define INC_3_COMBATCOMPONENT_H

#include "IComponent.h"
#include <algorithm>

namespace game::entity::components {

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
    [[nodiscard]] double get_base_accuracy() const override { return accuracy_; }
    void set_base_accuracy(double acc) override { accuracy_ = std::clamp(acc, 0.0, 1.0); }

protected:
    double accuracy_ = 0;
};

}

#endif //INC_3_COMBATCOMPONENT_H
