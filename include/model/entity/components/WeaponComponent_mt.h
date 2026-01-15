#ifndef MYGAMEPROJECT_WEAPONCOMPONENT_MT_H
#define MYGAMEPROJECT_WEAPONCOMPONENT_MT_H

#include "IComponent_mt.h"
#include "model/entity/entities/items/Weapon_mt.h"
#include <memory>
#include <mutex>

namespace game::mt::entity::components {

/// @brief Класс компонента оружия
class WeaponComponent : public IComponent {
public:
    /**
     * @brief Метод получения текущего оружия
     * @return Указатель на оружие или nullptr, если оружие не задано
     */
    [[nodiscard]] virtual std::shared_ptr<game::mt::entity::items::Weapon> get_weapon() const = 0;
    /**
     * @brief Метод задания оружия
     * @param weapon Указатель на новое оружие
     * @return unique_ptr на прежнее оружие (может быть nullptr)
     */
    virtual std::shared_ptr<game::mt::entity::items::Weapon> set_weapon(
        std::shared_ptr<game::mt::entity::items::Weapon> weapon
    ) = 0;
    ~WeaponComponent() override = default;
};

class DefaultWeaponComp : public WeaponComponent {
public:
    DefaultWeaponComp() = default;
    explicit DefaultWeaponComp(std::shared_ptr<game::mt::entity::items::Weapon> w) : weapon_(std::move(w)) {}

    [[nodiscard]] std::shared_ptr<game::mt::entity::items::Weapon> get_weapon() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return weapon_;
    }
    std::shared_ptr<game::mt::entity::items::Weapon> set_weapon(
        std::shared_ptr<game::mt::entity::items::Weapon> weapon
    ) override;

protected:
    mutable std::mutex mutex_;
    std::shared_ptr<game::mt::entity::items::Weapon> weapon_;
};

}

#endif //MYGAMEPROJECT_WEAPONCOMPONENT_MT_H
