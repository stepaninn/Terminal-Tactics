#ifndef INC_3_AICOMPONENT_H
#define INC_3_AICOMPONENT_H

#include "IComponent.h"
#include "../../../types.h"

namespace game {

class AIComponent : public IComponent {
public:
    [[nodiscard]] virtual AIState get_state() const = 0;
    virtual void set_state(AIState s) = 0;
    ~AIComponent() override = default;
};

class DefaultAIComp : public AIComponent {
public:
    DefaultAIComp() = default;
    explicit DefaultAIComp(AIState s) : state_(s) {}
    [[nodiscard]] AIState get_state() const override { return state_; }
    void set_state(AIState s) override { state_ = s; }

protected:
    AIState state_ = AIState::NEUTRAL;
};

}

#endif //INC_3_AICOMPONENT_H