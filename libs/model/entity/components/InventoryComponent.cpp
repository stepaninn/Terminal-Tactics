#include "model/entity/components/InventoryComponent.h"

#include <algorithm>

#include "model/entity/entities/items/Item.h"

#include <stdexcept>

namespace game {

void DefaultInventoryComp::add(std::unique_ptr<Item> item) {
    if (!can_add(*item)) {
        throw std::runtime_error("No enough weight");
    }

    int w = item->get_weight();
    items_.push_back(std::move(item));
    weight_ += w;
}

std::unique_ptr<Item> DefaultInventoryComp::remove(const Item* ptr) {
    auto it = std::ranges::find_if(
        items_,
        [ptr](const std::unique_ptr<Item>& item) {
            return item.get() == ptr;
        }
    );

    if (it == items_.end()) return nullptr;

    int w = (*it)->get_weight();
    auto res = std::move(*it);
    items_.erase(it);
    weight_ -= w;
    return res;
}

std::unique_ptr<Item> DefaultInventoryComp::remove_at(size_t idx) {
    if (idx >= items_.size()) return nullptr;

    auto it = items_.begin() + idx;

    int w = (*it)->get_weight();
    auto res = std::move(*it);
    items_.erase(it);
    weight_ -= w;
    return res;
}

std::vector<const Item*> DefaultInventoryComp::get_items() const {
    std::vector<const Item*> res;
    res.reserve(items_.size());
    std::ranges::for_each(items_, [&res](const auto& item) {
        res.push_back(item.get());
    });
    return res;
}

}
