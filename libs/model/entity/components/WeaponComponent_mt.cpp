#include "model/entity/components/WeaponComponent_mt.h"

#include <mutex>

namespace game::mt::entity::components {

std::shared_ptr<game::mt::entity::items::Weapon> DefaultWeaponComp::set_weapon(
    std::shared_ptr<game::mt::entity::items::Weapon> weapon
) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto tmp = std::move(weapon_);
    weapon_ = std::move(weapon);
    return tmp;
}

}
