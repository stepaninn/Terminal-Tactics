#ifndef INC_3_HEALTHCOMPONENT_H
#define INC_3_HEALTHCOMPONENT_H

#include "IComponent.h"
#include "../types.h"

namespace game {

class HealthComponent : public IComponent {
public:
    [[nodiscard]] virtual int get_current_hp() const noexcept = 0;
    [[nodiscard]] virtual int get_max_hp() const noexcept = 0;
    virtual int add_hp(int amount) = 0;
    virtual int reduce_hp(int amount) = 0;
    ~HealthComponent() override = default;
};

class DefaultHealthComp : public HealthComponent {
public:
    DefaultHealthComp() = default;
    DefaultHealthComp(int cur, int maxv) : current_hp_(cur), max_hp_(maxv) {}

    [[nodiscard]] int get_current_hp() const noexcept override { return current_hp_; }
    [[nodiscard]] int get_max_hp() const noexcept override { return max_hp_; }

    // возвращает полученное hp
    [[nodiscard]] int add_hp(int amount) override;
    [[nodiscard]] int reduce_hp(int amount) override;

protected:
    int current_hp_ = 0;
    int max_hp_ = 0;
};

} // namespace game

#endif