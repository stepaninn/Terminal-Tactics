#include "model/entity/entities/items/Weapon.h"

namespace game {

int Weapon::add_ammo(int ammo) {
    int added = std::min(max_ammo_ - current_ammo_, ammo);
    current_ammo_ += added;
    return added;
}

int Weapon::reduce_ammo(int ammo) {
    int removed = std::min(current_ammo_, ammo);
    current_ammo_ -= removed;
    return removed;
}


}