#ifndef INC_3_TIMEPOINTSCOMPONENT_H
#define INC_3_TIMEPOINTSCOMPONENT_H

#include "IComponent.h"
#include <algorithm>

namespace game {

class TimePointsComponent : public IComponent {
public:
    [[nodiscard]] virtual int get_current_points() const = 0;
    virtual int add_points(int amount) = 0;
    virtual int reduce_points(int amount) = 0;
    [[nodiscard]] virtual int get_max_points() const = 0;
    ~TimePointsComponent() override = default;
};

class DefaultTimePointsComp : public TimePointsComponent {
public:
    DefaultTimePointsComp() = default;
    DefaultTimePointsComp(int cur, int maxv) : current_tp_(cur), max_tp_(maxv) {}

    [[nodiscard]] int get_current_points() const override { return current_tp_; }

    int add_points(int amount) override;
    int reduce_points(int amount) override;
    [[nodiscard]] int get_max_points() const override { return max_tp_; }

protected:
    int current_tp_ = 0;
    int max_tp_ = 0;
};

}

#endif //INC_3_TIMEPOINTSCOMPONENT_H