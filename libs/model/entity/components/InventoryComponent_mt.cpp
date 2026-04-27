#include "model/entity/components/InventoryComponent_mt.h"

#include <algorithm>
#include <mutex>

#include "model/entity/entities/items/Item_mt.h"

namespace game::mt::entity::components {

bool DefaultInventoryComp::can_add(const game::mt::entity::items::Item& item) const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (items_.size() >= static_cast<size_t>(capacity_)) return false;
    return item.get_weight() + weight_ <= max_weight_;
}

void DefaultInventoryComp::add(std::shared_ptr<game::mt::entity::items::Item> item) {
    if (!item) return;

    std::lock_guard<std::mutex> lock(mutex_);
    if (items_.size() >= static_cast<size_t>(capacity_)) return;
    int w = item->get_weight();
    if (w + weight_ > max_weight_) return;

    game::mt::id_t id = item->get_id();
    tbb::concurrent_hash_map<game::mt::id_t, std::shared_ptr<game::mt::entity::items::Item>>::accessor acc;
    if (items_.find(acc, id)) return;
    if (items_.insert(acc, id)) {
        acc->second = std::move(item);
    }

    weight_ += w;
}

std::shared_ptr<game::mt::entity::items::Item> DefaultInventoryComp::remove_by_id(game::mt::id_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    tbb::concurrent_hash_map<game::mt::id_t, std::shared_ptr<game::mt::entity::items::Item>>::accessor acc;
    if (!items_.find(acc, id)) return nullptr;

    int w = acc->second->get_weight();
    auto res = std::move(acc->second);
    items_.erase(acc);
    weight_ -= w;
    return res;
}

std::vector<std::shared_ptr<const game::mt::entity::items::Item>> DefaultInventoryComp::get_items() const {
    std::vector<std::shared_ptr<const game::mt::entity::items::Item>> res;
    res.reserve(items_.size());
    std::ranges::for_each(items_, [&res](const auto& item) {
        res.push_back(item.second);
    });
    return res;
}

std::shared_ptr<game::mt::entity::items::Item> DefaultInventoryComp::get_item(game::mt::id_t id) noexcept {
    tbb::concurrent_hash_map<game::mt::id_t, std::shared_ptr<game::mt::entity::items::Item>>::const_accessor acc;
    if (!items_.find(acc, id)) return nullptr;

    return acc->second;
}

std::shared_ptr<const game::mt::entity::items::Item> DefaultInventoryComp::get_item(game::mt::id_t id) const noexcept {
    tbb::concurrent_hash_map<game::mt::id_t, std::shared_ptr<game::mt::entity::items::Item>>::const_accessor acc;
    if (!items_.find(acc, id)) return nullptr;

    return acc->second;
}

}
