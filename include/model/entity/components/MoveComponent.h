#ifndef INC_3_MOVECOMPONENT_H
#define INC_3_MOVECOMPONENT_H

#include "IComponent.h"

namespace game {

class MoveComponent : public IComponent {
public:
    using Ptr = std::shared_ptr<MoveComponent>;
    virtual int get_step_cost() const = 0;
    virtual ~MoveComponent() = default;
};

class DefaultMoveComp : public MoveComponent {
public:
    DefaultMoveComp() = default;
    explicit DefaultMoveComp(int cost) : step_cost_(cost) {}
    int get_step_cost() const override { return step_cost_; }

protected:
    int step_cost_ = 1;
};

}

#endif //INC_3_MOVECOMPONENT_H