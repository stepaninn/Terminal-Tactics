#include "model/entity/components/WeaponComponent.h"

namespace game::entity::components {

std::unique_ptr<game::entity::items::Weapon> DefaultWeaponComp::set_weapon(
    std::unique_ptr<game::entity::items::Weapon> weapon
) {
    auto tmp = std::move(weapon_);
    weapon_ = std::move(weapon);
    return tmp;
}

}
