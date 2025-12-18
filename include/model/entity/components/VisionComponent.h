#ifndef INC_3_VISIONCOMPONENT_H
#define INC_3_VISIONCOMPONENT_H

#include "IComponent.h"
#include <algorithm>

namespace game {

class VisionComponent : public IComponent {
public:
    [[nodiscard]] virtual int get_vision_radius() const = 0;
    virtual void set_vision_radius(int r) = 0;

    [[nodiscard]] virtual bool is_sees_creatures() const = 0;

    ~VisionComponent() override = default;
};

class DefaultVisionComp : public VisionComponent {
public:
    DefaultVisionComp() = default;
    explicit DefaultVisionComp(int r, bool sees_items_only = false)
        : radius_(std::max(0, r)), sees_items_only_(sees_items_only) {}
    [[nodiscard]] int get_vision_radius() const override { return radius_; }

    void set_vision_radius(int r) override { radius_ = std::max(0, r); }

    [[nodiscard]] bool is_sees_creatures() const override { return !sees_items_only_; }

protected:
    int radius_ = 0;
    bool sees_items_only_ = false;
};

}

#endif //INC_3_VISIONCOMPONENT_H