#ifndef INC_3_CELL_H
#define INC_3_CELL_H

#include "Types.h"
#include <vector>
#include <memory>

namespace game {

class Cell {
public:
    Cell() = default;
    explicit Cell(CellType t) noexcept : type_(type) {}

    bool is_walkable() const noexcept;
    bool is_blocks_vision() const noexcept;

    CellType get_type() const noexcept { return type_; }
    void set_type(CellType new_type) noexcept { type_ = new_type; }

    const std::vector<std::shared_ptr<Item>>& get_items() const noexcept { return items_; }

    void add_item(std::shared_ptr<Item> new_item);

    std::shared_ptr<Item> pop_item(const std::shared_ptr<Item>& item);

private:
    CellType type_{CellType::FLOOR};
    std::vector<std::shared_ptr<Item>> items_;
};

} // namespace game


#endif //INC_3_CELL_H