#ifndef INC_3_POSITIONCOMPONENT_H
#define INC_3_POSITIONCOMPONENT_H

#include "IComponent.h"
#include "types.h"

namespace game {

class PositionComponent : public IComponent {
public:
    using Ptr = std::shared_ptr<PositionComponent>;
    virtual Position get_position() const = 0;
    virtual ActionResult set_position(const Position& pos) = 0;
    virtual ~PositionComponent() = default;
};

class DefaultPositionComp : public PositionComponent {
public:
    DefaultPositionComp() = default;
    explicit DefaultPositionComp(const Position& pos) : pos_(p) {}

    Position get_position() const override { return pos_; }
    ActionResult set_position(const Position& pos) override;

protected:
    Position pos_{0, 0};
};

} // namespace game

#endif //INC_3_POSITIONCOMPONENT_H