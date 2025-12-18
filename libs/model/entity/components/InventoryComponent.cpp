#include "model/entity/components/InventoryComponent.h"

#include <algorithm>

#include "model/entity/entities/items/Item.h"

#include <stdexcept>

namespace game {

bool DefaultInventoryComp::can_add(const Item& item) const noexcept {
    if (items_.size() == capacity_) return false;
    return item.get_weight() + weight_ <= max_weight_;
}

void DefaultInventoryComp::add(std::unique_ptr<Item> item, id_t id) {
    if (items_.size() == capacity_) throw std::runtime_error("Not enough space");
    if (item->get_weight() + weight_ >= max_weight_) throw std::runtime_error("Item is too heavy");

    int w = item->get_weight();
    items_[id] = std::move(item);
    weight_ += w;
}

std::unique_ptr<Item> DefaultInventoryComp::remove_by_id(id_t id) {
    auto it = items_.find(id);

    if (it == items_.end()) return nullptr;

    int w = (it->second)->get_weight();
    auto res = std::move(it->second);
    items_.erase(it);
    weight_ -= w;
    return res;
}

std::vector<const Item*> DefaultInventoryComp::get_items() const {
    std::vector<const Item*> res;
    res.reserve(items_.size());
    std::ranges::for_each(items_, [&res](const auto& item) {
        res.push_back(item.second.get());
    });
    return res;
}

}
