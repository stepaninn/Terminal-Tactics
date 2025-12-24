#ifndef INC_3_MOVECOMPONENT_H
#define INC_3_MOVECOMPONENT_H

#include "IComponent.h"
#include <algorithm>

namespace game::entity::components {

class MoveComponent : public IComponent {
public:
    [[nodiscard]] virtual int get_step_cost() const noexcept = 0;
    virtual void set_step_cost(int new_cost) = 0;
    ~MoveComponent() override = default;
};

class DefaultMoveComp : public MoveComponent {
public:
    DefaultMoveComp() = default;
    explicit DefaultMoveComp(int cost) : step_cost_(cost) {}
    [[nodiscard]] int get_step_cost() const noexcept override { return step_cost_; }
    void set_step_cost(int new_cost) override { step_cost_ = std::max(1, new_cost); }

protected:
    int step_cost_ = 1;
};

}

#endif //INC_3_MOVECOMPONENT_H
