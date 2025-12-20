#include "model/repository/cells/Cell.h"

#include <algorithm>

namespace game {

std::unique_ptr<Item> ItemStorage::remove_by_id(id_t id) {
    auto it = items_.find(id);
    if (it == items_.end()) return nullptr;

    auto res = std::move(it->second);
    items_.erase(it);
    return res;
}

std::vector<const Item*> ItemStorage::get_items() const {
    std::vector<const Item*> res;
    res.reserve(items_.size());
    std::ranges::for_each(items_, [&res](const auto& item) {
        res.push_back(item.second.get());
    });
    return res;
}

const Item* ItemStorage::get_item(id_t id) const noexcept {
    auto it = items_.find(id);
    if (it == items_.end()) return nullptr;

    return it->second.get();
}

}