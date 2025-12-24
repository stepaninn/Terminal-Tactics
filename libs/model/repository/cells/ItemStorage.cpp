#include "model/repository/cells/Cell.h"

#include <algorithm>

namespace game::repo::cells {

std::unique_ptr<game::entity::items::Item> ItemStorage::remove_by_id(game::ItemId id) {
    auto it = items_.find(id);
    if (it == items_.end()) return nullptr;

    auto res = std::move(it->second);
    items_.erase(it);
    return res;
}

std::vector<const game::entity::items::Item*> ItemStorage::get_items() const {
    std::vector<const game::entity::items::Item*> res;
    res.reserve(items_.size());
    std::ranges::for_each(items_, [&res](const auto& item) {
        res.push_back(item.second.get());
    });
    return res;
}

const game::entity::items::Item* ItemStorage::get_item(game::ItemId id) const noexcept {
    auto it = items_.find(id);
    if (it == items_.end()) return nullptr;

    return it->second.get();
}

}
