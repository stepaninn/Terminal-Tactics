#include "model/entity/entities/items/Weapon_mt.h"

namespace game::mt::entity::items {

int Weapon::add_ammo(int ammo) {
    std::lock_guard<std::mutex> lock(mutex_);
    int added = std::min(max_ammo_ - current_ammo_, ammo);
    current_ammo_ += added;
    return added;
}

int Weapon::reduce_ammo(int ammo) {
    std::lock_guard<std::mutex> lock(mutex_);
    int removed = std::min(current_ammo_, ammo);
    current_ammo_ -= removed;
    return removed;
}


}
