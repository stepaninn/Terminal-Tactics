#ifndef INC_3_VISIONCOMPONENT_H
#define INC_3_VISIONCOMPONENT_H

#include "IComponent.h"

namespace game {

class VisionComponent : public IComponent {
public:
    using Ptr = std::shared_ptr<VisionComponent>;
    virtual int get_vision_radius() const = 0;
    virtual void set_vision_radius(int r) = 0;

    virtual bool sees(const Entity& target) const = 0;

    virtual ~VisionComponent() = default;
};

class DefaultVisionComp : public VisionComponent {
public:
    DefaultVisionComp() = default;
    explicit DefaultVisionComp(int r, bool sees_items_only = false) : radius_(r) {}
    int get_vision_radius() const override { return radius_; }
    void set_vision_radius(int r) override { radius_ = r; }

protected:
    int radius_ = 0;
    bool sees_items_only_ = false;
};

}

#endif //INC_3_VISIONCOMPONENT_H