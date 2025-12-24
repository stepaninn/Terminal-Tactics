#include "model/entity/components/TimePointsComponent.h"

namespace game::entity::components {

int DefaultTimePointsComp::add_points(int amount) {
    int added = std::min(max_tp_ - current_tp_, amount);
    current_tp_ += added;
    return added;
}

int DefaultTimePointsComp::reduce_points(int amount) {
    if (amount > current_tp_) return 0;
    current_tp_ -= amount;
    return amount;
}


}
