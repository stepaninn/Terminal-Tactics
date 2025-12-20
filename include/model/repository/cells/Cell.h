#ifndef INC_3_CELL_H
#define INC_3_CELL_H

#include "model/entity/entities/items/Item.h"
#include "types.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string_view>

namespace game {

class ICell {
public:
    virtual ~ICell() = default;
    [[nodiscard]] virtual bool is_walkable() const noexcept = 0;
    [[nodiscard]] virtual bool is_blocks_vision() const noexcept = 0;
    [[nodiscard]] virtual bool can_shoot_through() const noexcept = 0;
    [[nodiscard]] virtual bool apply_shot() noexcept { return false; }

    [[nodiscard]] virtual bool can_place_items() const noexcept { return false; }
    [[nodiscard]] virtual const Item* get_item(id_t id) const { return nullptr; }
    [[nodiscard]] virtual std::vector<const Item*> get_items() const { return {}; }

    [[nodiscard]] virtual std::string_view view_name() const noexcept = 0;

    virtual void add([[maybe_unused]] std::unique_ptr<Item> item, [[maybe_unused]] id_t id) {}
    virtual std::unique_ptr<Item> remove_by_id([[maybe_unused]] id_t id) { return nullptr; }

    [[nodiscard]] virtual size_t size() const noexcept { return 0; }
    
};

struct ItemStorage {
    std::unordered_map<id_t, std::unique_ptr<Item>> items_;

    void add(std::unique_ptr<Item> item, id_t id) { items_[id] = std::move(item); }

    std::unique_ptr<Item> remove_by_id(id_t id);

    size_t size() const noexcept { return items_.size(); }

    std::vector<const Item*> get_items() const;

    const Item* get_item(id_t id) const noexcept;
};

}


#endif //INC_3_CELL_H
