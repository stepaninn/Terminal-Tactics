#include "model/entity/components/WeaponComponent.h"

namespace game {

std::shared_ptr<Weapon> DefaultWeaponComp::set_weapon(const std::shared_ptr<Weapon> weapon) {
    auto tmp = weapon;
    weapon_ = weapon;
    return tmp;
}

}