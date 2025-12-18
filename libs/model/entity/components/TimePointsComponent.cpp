#include "model/entity/components/TimePointsComponent.h"

namespace game {

int DefaultTimePointsComp::add_points(int amount) {
    int added = std::min(max_tp_ - current_tp_, amount);
    current_tp_ += added;
    return added;
}

int DefaultTimePointsComp::reduce_points(int amount) {
    int removed = std::min(current_tp_, amount);
    current_tp_ -= removed;
    return removed;
}


}