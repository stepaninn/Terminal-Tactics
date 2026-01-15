#ifndef MYGAMEPROJECT_VISIONCOMPONENT_MT_H
#define MYGAMEPROJECT_VISIONCOMPONENT_MT_H

#include "IComponent_mt.h"
#include <algorithm>
#include <mutex>

namespace game::mt::entity::components {

/// @brief Класс компонента зрения
class VisionComponent : public IComponent {
public:
    /**
     * @brief Метод получения радиуса обзора
     * @return int радиус обзора
     */
    [[nodiscard]] virtual int get_vision_radius() const = 0;
    /**
     * @brief Метод задания радиуса обзора
     * @param r Новый радиус обзора (не меньше нуля)
     */
    virtual void set_vision_radius(int r) = 0;

    /**
     * @brief Метод проверки видимости существ
     * @return bool true, если компонент не видит существ, иначе false
     */
    [[nodiscard]] virtual bool is_sees_items_only() const = 0;

    ~VisionComponent() override = default;
};

class DefaultVisionComp : public VisionComponent {
public:
    DefaultVisionComp() = default;
    explicit DefaultVisionComp(int r, bool sees_items_only = false)
        : radius_(std::max(0, r)), sees_items_only_(sees_items_only) {}
    [[nodiscard]] int get_vision_radius() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return radius_;
    }

    void set_vision_radius(int r) override {
        std::lock_guard<std::mutex> lock(mutex_);
        radius_ = std::max(0, r);
    }

    [[nodiscard]] bool is_sees_items_only() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return sees_items_only_;
    }

protected:
    mutable std::mutex mutex_;
    int radius_ = 0;
    bool sees_items_only_ = false;
};

}

#endif //MYGAMEPROJECT_VISIONCOMPONENT_MT_H
