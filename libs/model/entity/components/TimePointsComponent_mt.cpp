#include "model/entity/components/TimePointsComponent_mt.h"

#include <mutex>

namespace game::mt::entity::components {

int DefaultTimePointsComp::add_points(int amount) {
    std::lock_guard<std::mutex> lock(mutex_);
    int added = std::min(max_tp_ - current_tp_, amount);
    current_tp_ += added;
    return added;
}

int DefaultTimePointsComp::reduce_points(int amount) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (amount > current_tp_) return 0;
    current_tp_ -= amount;
    return amount;
}

}
