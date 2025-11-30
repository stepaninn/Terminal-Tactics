#ifndef INC_3_MOVECOMPONENT_H
#define INC_3_MOVECOMPONENT_H

#include "IComponent.h"

namespace game {

class MoveComponent : public IComponent {
public:
    [[nodiscard]] virtual int get_step_cost() const noexcept = 0;
    virtual void set_step_cost(double new_cost);
    virtual ~MoveComponent() = default;
};

class DefaultMoveComp : public MoveComponent {
public:
    DefaultMoveComp() = default;
    explicit DefaultMoveComp(double cost) : step_cost_(cost) {}
    [[nodiscard]] double get_step_cost() const noexcept override { return step_cost_; }
    void set_step_cost(double new_cost);

protected:
    double step_cost_ = 1;
};

}

#endif //INC_3_MOVECOMPONENT_H