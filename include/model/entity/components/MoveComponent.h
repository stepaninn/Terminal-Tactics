#ifndef INC_3_MOVECOMPONENT_H
#define INC_3_MOVECOMPONENT_H

#include "IComponent.h"
#include <algorithm>

namespace game::entity::components {

/// @brief Класс компонента перемещения
class MoveComponent : public IComponent {
public:
    /**
     * @brief Метод получения стоимости шага
     * @return int стоимость шага (в очках времени)
     */
    [[nodiscard]] virtual int get_step_cost() const noexcept = 0;
    /**
     * @brief Метод задания стоимости шага
     * @param new_cost Новая стоимость шага (минимум 1)
     */
    virtual void set_step_cost(int new_cost) = 0;
    ~MoveComponent() override = default;
};

class DefaultMoveComp : public MoveComponent {
public:
    DefaultMoveComp() = default;
    explicit DefaultMoveComp(int cost) : step_cost_(cost) {}
    [[nodiscard]] int get_step_cost() const noexcept override { return step_cost_; }
    void set_step_cost(int new_cost) override { step_cost_ = std::max(1, new_cost); }

protected:
    int step_cost_ = 1;
};

}

#endif //INC_3_MOVECOMPONENT_H
