#ifndef MYGAMEPROJECT_AICOMPONENT_H
#define MYGAMEPROJECT_AICOMPONENT_H

#include "IComponent.h"
#include "../../../types.h"

namespace game::entity::components {

enum class AIBehavior {
    WILD,
    INTELLIGENT,
    FORAGER
};

/// @brief Класс компонента ИИ
class AIComponent : public IComponent {
public:
    /**
     * @brief Метод получения типа поведения ИИ
     * @return AIBehaviorType тип поведения
     */
    [[nodiscard]] virtual AIBehavior get_behavior() const = 0;
    /**
     * @brief Метод задания типа поведения ИИ
     * @param t тип поведения
     */
    virtual void set_behavior(AIBehavior b) = 0;
    ~AIComponent() override = default;
};

class DefaultAIComp : public AIComponent {
public:
    DefaultAIComp() = default;
    explicit DefaultAIComp(AIBehavior b = AIBehavior::WILD) : behavior_(b) {}
    [[nodiscard]] AIBehavior get_behavior() const override { return behavior_; }
    void set_behavior(AIBehavior b) override { behavior_ = b; }

protected:
    AIBehavior behavior_ = AIBehavior::WILD;
};

}

#endif //MYGAMEPROJECT_AICOMPONENT_H
