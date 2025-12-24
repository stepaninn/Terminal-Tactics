#include "model/entity/components/WeaponComponent.h"

namespace game::entity::components {

std::shared_ptr<game::entity::items::Weapon> DefaultWeaponComp::set_weapon(
    const std::shared_ptr<game::entity::items::Weapon> weapon
) {
    auto tmp = weapon_;
    weapon_ = weapon;
    return tmp;
}

}
