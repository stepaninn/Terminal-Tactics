#ifndef INC_3_HEALTHCOMPONENT_H
#define INC_3_HEALTHCOMPONENT_H

#include "IComponent.h"
#include "types.h"

namespace game {

class HealthComponent : public IComponent {
public:
    using Ptr = std::shared_ptr<HealthComponent>;
    virtual int get_current_hp() const noexcept = 0;
    virtual int get_max_hp() const noexcept = 0;
    virtual void set_current_hp(int new_hp) noexcept = 0;
    ~HealthComponent() override = default;
};

class DefaultHealthComp : public HealthComponent {
public:
    DefaultHealthComp() = default;
    DefaultHealthComp(int cur, int maxv) : current_hp_(cur), max_hp_(maxv) {}

    int get_current_hp() const noexcept override { return current_hp_; }
    int get_max_hp() const noexcept override { return max_hp_; }
    void set_current_hp(int new_hp) noexcept override { current_hp_ = new_hp; }

protected:
    int current_hp_ = 0;
    int max_hp_ = 0;
};

} // namespace game

#endif