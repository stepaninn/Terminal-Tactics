#ifndef INC_3_WEAPONCOMPONENT_H
#define INC_3_WEAPONCOMPONENT_H

#include "IComponent.h"
#include <memory>

namespace game::entity::items {
class Weapon;
}

namespace game::entity::components {

class WeaponComponent : public IComponent {
public:
    [[nodiscard]] virtual std::shared_ptr<game::entity::items::Weapon> get_weapon() const = 0;
    // возвращает прежнее оружие
    virtual std::shared_ptr<game::entity::items::Weapon> set_weapon(
        std::shared_ptr<game::entity::items::Weapon> weapon
    ) = 0;
    ~WeaponComponent() override = default;
};

class DefaultWeaponComp : public WeaponComponent {
public:
    DefaultWeaponComp() = default;
    explicit DefaultWeaponComp(std::shared_ptr<game::entity::items::Weapon> w) : weapon_(std::move(w)) {}

    [[nodiscard]] std::shared_ptr<game::entity::items::Weapon> get_weapon() const override { return weapon_; }
    std::shared_ptr<game::entity::items::Weapon> set_weapon(
        std::shared_ptr<game::entity::items::Weapon> weapon
    ) override;

protected:
    std::shared_ptr<game::entity::items::Weapon> weapon_;
};

}

#endif //INC_3_WEAPONCOMPONENT_H
