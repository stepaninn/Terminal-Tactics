#ifndef MYGAMEPROJECT_WEAPONCOMPONENT_H
#define MYGAMEPROJECT_WEAPONCOMPONENT_H

#include "IComponent.h"
#include "model/entity/entities/items/Weapon.h"
#include <memory>

namespace game::entity::components {

/// @brief Класс компонента оружия
class WeaponComponent : public IComponent {
public:
    /**
     * @brief Метод получения текущего оружия
     * @return Указатель на оружие или nullptr, если оружие не задано
     */
    [[nodiscard]] virtual game::entity::items::Weapon* get_weapon() const = 0;
    /**
     * @brief Метод задания оружия
     * @param weapon Указатель на новое оружие
     * @return unique_ptr на прежнее оружие (может быть nullptr)
     */
    virtual std::unique_ptr<game::entity::items::Weapon> set_weapon(
        std::unique_ptr<game::entity::items::Weapon> weapon
    ) = 0;
    ~WeaponComponent() override = default;
};

class DefaultWeaponComp : public WeaponComponent {
public:
    DefaultWeaponComp() = default;
    explicit DefaultWeaponComp(std::unique_ptr<game::entity::items::Weapon> w) : weapon_(std::move(w)) {}

    [[nodiscard]] game::entity::items::Weapon* get_weapon() const override { return weapon_.get(); }
    std::unique_ptr<game::entity::items::Weapon> set_weapon(
        std::unique_ptr<game::entity::items::Weapon> weapon
    ) override;

protected:
    std::unique_ptr<game::entity::items::Weapon> weapon_;
};

}

#endif //MYGAMEPROJECT_WEAPONCOMPONENT_H
