#include "model/repository/cells/ItemContainer_mt.h"

#include <algorithm>

namespace game::mt::repo::cells {

std::shared_ptr<game::mt::entity::items::Item> ItemStorage::remove_by_id(game::mt::ItemId id) {
    tbb::concurrent_hash_map<game::mt::ItemId, std::shared_ptr<game::mt::entity::items::Item>>::accessor acc;
    if (!items_.find(acc, id)) return nullptr;

    auto res = std::move(acc->second);
    items_.erase(acc);
    return res;
}

std::vector<std::shared_ptr<const game::mt::entity::items::Item>> ItemStorage::get_items() const {
    std::vector<std::shared_ptr<const game::mt::entity::items::Item>> res;
    res.reserve(items_.size());
    std::ranges::for_each(items_, [&res](const auto& item) {
        res.push_back(item.second);
    });
    return res;
}

std::shared_ptr<const game::mt::entity::items::Item> ItemStorage::get_item(game::mt::ItemId id) const noexcept {
    tbb::concurrent_hash_map<game::mt::ItemId, std::shared_ptr<game::mt::entity::items::Item>>::const_accessor acc;
    if (!items_.find(acc, id)) return nullptr;

    return acc->second;
}

}
