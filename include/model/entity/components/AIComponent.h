#ifndef INC_3_AICOMPONENT_H
#define INC_3_AICOMPONENT_H

#include "IComponent.h"
#include "../../../types.h"

namespace game::entity::components {

class AIComponent : public IComponent {
public:
    [[nodiscard]] virtual game::AIState get_state() const = 0;
    virtual void set_state(game::AIState s) = 0;
    ~AIComponent() override = default;
};

class DefaultAIComp : public AIComponent {
public:
    DefaultAIComp() = default;
    explicit DefaultAIComp(game::AIState s) : state_(s) {}
    [[nodiscard]] game::AIState get_state() const override { return state_; }
    void set_state(game::AIState s) override { state_ = s; }

protected:
    game::AIState state_ = game::AIState::NEUTRAL;
};

}

#endif //INC_3_AICOMPONENT_H
