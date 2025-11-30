A#ifndef INC_3_TIMEPOINTSCOMPONENT_H
#define INC_3_TIMEPOINTSCOMPONENT_H

#include "IComponent.h"

namespace game {

class TimePointsComponent : public IComponent {
public:
    [[nodiscard]] virtual int get_current_points() const = 0;
    virtual void set_current_points(int tp) = 0;
    [[nodiscard]] virtual int get_max_points() const = 0;
    virtual ~TimePointsComponent() = default;
};

class DefaultTimePointsComp : public TimePointsComponent {
public:
    DefaultTimePointsComp() = default;
    DefaultTimePointsComp(int cur, int maxv) : current_time_points_(cur), max_time_points_(maxv) {}

    [[nodiscard]] int get_current_points() const override { return current_time_points_; }
    // возвращают исключение, если недействительно
    void set_current_points(int tp) override;
    [[nodiscard]] int get_max_points() const override { return max_time_points_; }

protected:
    int current_time_points_ = 0;
    int max_time_points_ = 0;
};

}

#endif //INC_3_TIMEPOINTSCOMPONENT_H