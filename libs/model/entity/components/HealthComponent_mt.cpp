#include "model/entity/components/HealthComponent_mt.h"

#include <algorithm>
#include <mutex>

namespace game::mt::entity::components {

int DefaultHealthComp::add_hp(int amount) {
    std::lock_guard<std::mutex> lock(mutex_);
    int added = std::min(max_hp_ - current_hp_, amount);
    current_hp_ += added;
    return added;
}

int DefaultHealthComp::reduce_hp(int amount) {
    std::lock_guard<std::mutex> lock(mutex_);
    int removed = std::min(current_hp_, amount);
    current_hp_ -= removed;
    return removed;
}

}
