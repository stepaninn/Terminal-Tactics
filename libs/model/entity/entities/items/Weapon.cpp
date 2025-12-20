#include "model/entity/entities/items/Weapon.h"

namespace game {

int AmmoBag::add_ammo(int ammo) {
    int added = std::min(max_ammo_ - current_ammo_, ammo);
    current_ammo_ += added;
    return added;
}

int AmmoBag::reduce_ammo(int ammo) {
    int removed = std::min(current_ammo_, ammo);
    current_ammo_ -= removed;
    return removed;
}


}