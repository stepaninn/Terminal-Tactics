#ifndef INC_3_POSITIONCOMPONENT_H
#define INC_3_POSITIONCOMPONENT_H

#include "IComponent.h"
#include "types.h"

namespace game {

class PositionComponent : public IComponent {
public:
    [[nodiscard]] virtual Position get_position() const = 0;
    virtual void set_position(const Position& pos) = 0;
    virtual ~PositionComponent() = default;
};

class DefaultPositionComp : public PositionComponent {
public:
    DefaultPositionComp() = default;
    explicit DefaultPositionComp(const Position& pos) : pos_(p) {}

    [[nodiscard]] Position get_position() const override { return pos_; }
    void set_position(const Position& pos) override { pos_ = pos; }

protected:
    Position pos_{0, 0};
};

} // namespace game

#endif //INC_3_POSITIONCOMPONENT_H