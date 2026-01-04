#ifndef INC_3_CELL_H
#define INC_3_CELL_H

#include "model/entity/entities/items/Item.h"
#include "types.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string_view>

namespace game::repo::cells {

class ICell {
public:
    virtual ~ICell() = default;
    [[nodiscard]] virtual bool is_walkable() const noexcept = 0;
    [[nodiscard]] virtual bool is_blocks_vision() const noexcept = 0;
    [[nodiscard]] virtual bool can_shoot_through() const noexcept = 0;
    [[nodiscard]] virtual bool apply_shot() noexcept { return false; }

    [[nodiscard]] virtual bool can_place_items() const noexcept { return false; }
    [[nodiscard]] virtual const game::entity::items::Item* get_item(game::ItemId id) const { return nullptr; }
    [[nodiscard]] virtual std::vector<const game::entity::items::Item*> get_items() const { return {}; }

    [[nodiscard]] virtual std::string_view view_name() const noexcept = 0;

    virtual void add([[maybe_unused]] std::unique_ptr<game::entity::items::Item> item) {}
    virtual std::unique_ptr<game::entity::items::Item> remove_by_id([[maybe_unused]] game::ItemId id) {
        return nullptr;
    }

    [[nodiscard]] virtual size_t size() const noexcept { return 0; }
    
};

struct ItemStorage {
    std::unordered_map<game::ItemId, std::unique_ptr<game::entity::items::Item>> items_;

    void add(std::unique_ptr<game::entity::items::Item> item) {
        if (!item) return;
        items_[item->get_id()] = std::move(item);
    }

    std::unique_ptr<game::entity::items::Item> remove_by_id(game::ItemId id);

    size_t size() const noexcept { return items_.size(); }

    std::vector<const game::entity::items::Item*> get_items() const;

    const game::entity::items::Item* get_item(game::ItemId id) const noexcept;
};

}


#endif //INC_3_CELL_H
