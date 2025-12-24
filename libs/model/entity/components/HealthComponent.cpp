#include "model/entity/components/HealthComponent.h"

#include <algorithm>

namespace game::entity::components {

int DefaultHealthComp::add_hp(int amount) {
    int added = std::min(max_hp_ - current_hp_, amount);
    current_hp_ += added;
    return added;
}

int DefaultHealthComp::reduce_hp(int amount) {
    int removed = std::min(current_hp_, amount);
    current_hp_ -= removed;
    return removed;
}

}
