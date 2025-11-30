#ifndef INC_3_COMBATCOMPONENT_H
#define INC_3_COMBATCOMPONENT_H

#include "IComponent.h"

namespace game {

class CombatComponent : public IComponent {
public:
    [[nodiscard]] virtual int get_base_accuracy() const = 0;
    virtual void set_base_accuracy(int acc) = 0;
    virtual ~CombatComponent() = default;
};

class DefaultCombatComp : public CombatComponent {
public:
    DefaultCombatComp() = default;
    explicit DefaultCombatComp(int acc) : accuracy_(acc) {}
    [[nodiscard]] int get_base_accuracy() const override { return accuracy_; }
    void set_base_accuracy(int acc) override { accuracy_ = acc; }

protected:
    int accuracy_ = 0;
};

}

#endif //INC_3_COMBATCOMPONENT_H