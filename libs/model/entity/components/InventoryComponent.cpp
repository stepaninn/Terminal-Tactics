#include "model/entity/components/InventoryComponent.h"

#include <algorithm>

#include "model/entity/entities/items/Item.h"

namespace game::entity::components {

bool DefaultInventoryComp::can_add(const game::entity::items::Item& item) const noexcept {
    if (items_.size() == capacity_) return false;
    return item.get_weight() + weight_ <= max_weight_;
}

void DefaultInventoryComp::add(std::unique_ptr<game::entity::items::Item> item) {
    if (!item || !can_add(*item)) return;

    int w = item->get_weight();

    game::id_t id = item->get_id();
    auto it = items_.find(id);
    if (it != items_.end()) return;
    items_.emplace(id, std::move(item));

    weight_ += w;
}

std::unique_ptr<game::entity::items::Item> DefaultInventoryComp::remove_by_id(game::id_t id) {
    auto it = items_.find(id);

    if (it == items_.end()) return nullptr;

    int w = (it->second)->get_weight();
    auto res = std::move(it->second);
    items_.erase(it);
    weight_ -= w;
    return res;
}

std::vector<const game::entity::items::Item*> DefaultInventoryComp::get_items() const {
    std::vector<const game::entity::items::Item*> res;
    res.reserve(items_.size());
    std::ranges::for_each(items_, [&res](const auto& item) {
        res.push_back(item.second.get());
    });
    return res;
}

game::entity::items::Item* DefaultInventoryComp::get_item(game::id_t id) noexcept {
    auto it = items_.find(id);
    if (it == items_.end()) return nullptr;

    return it->second.get();
}

const game::entity::items::Item* DefaultInventoryComp::get_item(game::id_t id) const noexcept {
    auto it = items_.find(id);
    if (it == items_.end()) return nullptr;

    return it->second.get();
}

}
