#ifndef INC_3_POSITIONCOMPONENT_H
#define INC_3_POSITIONCOMPONENT_H

#include "IComponent.h"
#include "../../../types.h"

namespace game::entity::components {

class PositionComponent : public IComponent {
public:
    [[nodiscard]] virtual game::Position get_position() const = 0;
    virtual void set_position(game::Position pos) = 0;
    ~PositionComponent() override = default;
};

class DefaultPositionComp : public PositionComponent {
public:
    DefaultPositionComp() = default;
    explicit DefaultPositionComp(game::Position pos) : pos_(pos) {}

    [[nodiscard]] game::Position get_position() const override { return pos_; }
    void set_position(game::Position pos) override { pos_ = pos; }

protected:
    game::Position pos_{0, 0};
};

}

#endif //INC_3_POSITIONCOMPONENT_H
