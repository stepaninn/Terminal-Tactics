#ifndef INC_3_TIMEPOINTSCOMPONENT_H
#define INC_3_TIMEPOINTSCOMPONENT_H

#include "IComponent.h"
#include <algorithm>

namespace game::entity::components {

/// @brief Класс компонента очков времени
class TimePointsComponent : public IComponent {
public:
    /**
     * @brief Метод получения текущих очков времени
     * @return int количество текущих очков времени
     */
    [[nodiscard]] virtual int get_current_points() const = 0;
    /**
     * @brief Метод добавления очков времени
     * @param amount Количество добавляемых очков времени
     * @return int количество реально добавленных очков
     * @note Не может увеличить очки выше максимума
     */
    virtual int add_points(int amount) = 0;
    /**
     * @brief Метод снятия очков времени
     * @param amount Количество снимаемых очков времени
     * @return int количество реально снятых очков
     * @note Не может уменьшить очки меньше нуля
     */
    virtual int reduce_points(int amount) = 0;
    /**
     * @brief Метод получения максимальных очков времени
     * @return int количество максимальных очков времени
     */
    [[nodiscard]] virtual int get_max_points() const = 0;
    ~TimePointsComponent() override = default;
};

class DefaultTimePointsComp : public TimePointsComponent {
public:
    DefaultTimePointsComp() = default;
    DefaultTimePointsComp(int cur, int maxv) : current_tp_(std::max(0, cur)), max_tp_(std::max(0, maxv)) {
        if (current_tp_ > max_tp_) current_tp_ = max_tp_;
    }

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
