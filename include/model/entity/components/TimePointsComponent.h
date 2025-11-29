A#ifndef INC_3_TIMEPOINTSCOMPONENT_H
#define INC_3_TIMEPOINTSCOMPONENT_H

#include "IComponent.h"

namespace game {

class TimePointsComponent : public IComponent {
public:
    using Ptr = std::shared_ptr<TimePointsComponent>;
    virtual int get_current_points() const = 0;
    virtual void set_current_points(int tp) = 0;
    virtual int get_max_points() const = 0;
    virtual ~TimePointsComponent() = default;
};

class DefaultTimePointsComp : public TimePointsComponent {
public:
    DefaultTimePointsComp() = default;
    DefaultTimePointsComp(int cur, int maxv) : current_time_points_(cur), max_time_points_(maxv) {}

    int get_current_points() const override { return current_time_points_; }
    void set_current_points(int tp) override { current_time_points_ = tp; }
    int get_max_points() const override { return max_time_points_; }

protected:
    int current_time_points_ = 0;
    int max_time_points_ = 0;
};

}

#endif //INC_3_TIMEPOINTSCOMPONENT_H