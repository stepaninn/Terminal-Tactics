#ifndef MYGAMEPROJECT_AICOMPONENT_MT_H
#define MYGAMEPROJECT_AICOMPONENT_MT_H

#include "IComponent_mt.h"
#include <mutex>

namespace game::mt::entity::components {

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
     * @return AIBehavior тип поведения
     */
    [[nodiscard]] virtual AIBehavior get_behavior() const = 0;
    /**
     * @brief Метод задания типа поведения ИИ
     * @param b тип поведения
     */
    virtual void set_behavior(AIBehavior b) = 0;
    ~AIComponent() override = default;
};

class DefaultAIComp : public AIComponent {
public:
    DefaultAIComp() = default;
    explicit DefaultAIComp(AIBehavior b = AIBehavior::WILD) : behavior_(b) {}
    [[nodiscard]] AIBehavior get_behavior() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return behavior_;
    }
    void set_behavior(AIBehavior b) override {
        std::lock_guard<std::mutex> lock(mutex_);
        behavior_ = b;
    }

protected:
    mutable std::mutex mutex_;
    AIBehavior behavior_ = AIBehavior::WILD;
};

}

#endif //MYGAMEPROJECT_AICOMPONENT_MT_H
