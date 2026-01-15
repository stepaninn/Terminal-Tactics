#ifndef MYGAMEPROJECT_HEALTHCOMPONENT_MT_H
#define MYGAMEPROJECT_HEALTHCOMPONENT_MT_H

#include "IComponent_mt.h"
#include <algorithm>
#include <mutex>

namespace game::mt::entity::components {

/// @brief Класс компонента здоровья
class HealthComponent : public IComponent {
public:
    /**
     * @brief Метод получения текущего здоровья
     * @return int количество текущего здоровья
     */
    [[nodiscard]] virtual int get_current_hp() const noexcept = 0;
    /**
     * @brief Метод получения максимального здоровья
     * @return int количество максимального здоровья
     */
    [[nodiscard]] virtual int get_max_hp() const noexcept = 0;
    /**
     * @brief Метод добавления здоровья
     * @param amount Количество добавляемого здоровья
     * @return int количество реально добавленного здоровья
     * @note Не может увеличить здоровье выше максимума
     */
    virtual int add_hp(int amount) = 0;
    /**
     * @brief Метод снятия здоровья
     * @param amount Количество убираемого здоровья
     * @return Количество реально убранного здоровья
     * @note Не может уменьшить здоровье меньше нуля
     */
    virtual int reduce_hp(int amount) = 0;
    ~HealthComponent() override = default;
};

class DefaultHealthComp : public HealthComponent {
public:
    DefaultHealthComp() = default;
    DefaultHealthComp(int cur, int maxv)
        : current_hp_(std::max(0, cur)), max_hp_(std::max(0, maxv))
    {
        if (current_hp_ > max_hp_) current_hp_ = max_hp_;
    }

    [[nodiscard]] int get_current_hp() const noexcept override {
        std::lock_guard<std::mutex> lock(mutex_);
        return current_hp_;
    }
    [[nodiscard]] int get_max_hp() const noexcept override {
        std::lock_guard<std::mutex> lock(mutex_);
        return max_hp_;
    }

    // возвращает полученное hp
    [[nodiscard]] int add_hp(int amount) override;
    [[nodiscard]] int reduce_hp(int amount) override;

protected:
    mutable std::mutex mutex_;
    int current_hp_ = 0;
    int max_hp_ = 0;
};

}

#endif
