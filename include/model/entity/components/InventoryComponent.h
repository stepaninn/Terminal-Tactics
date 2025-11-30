#ifndef INC_3_INVENTORYCOMPONENT_H
#define INC_3_INVENTORYCOMPONENT_H

#include "IComponent.h"
#include "types.h"
#include <vector>

namespace game {

class Item;

class InventoryComponent : public IComponent {
public:
    [[nodiscard]] virtual ActionResult add(std::shared_ptr<Item> item) = 0;
    virtual std::shared_ptr<Item> remove(std::shared_ptr<Item> item) = 0;
    [[nodiscard]] virtual const std::vector<std::shared_ptr<Item>>& get_items() const = 0;
    [[nodiscard]] virtual std::shared_ptr<Item> get_item(std::size_t idx) const = 0;
    [[nodiscard]] virtual int get_weight() const = 0;
    [[nodiscard]] virtual int get_capacity() const = 0;
    [[nodiscard]] virtual bool can_add(std::shared_ptr<Item> item) const = 0;
    virtual ~InventoryComponent() = default;
};

class DefaultInventoryComp : public InventoryComponent {
public:
    DefaultInventoryComp(int capacity = 0) : capacity_(capacity), weight_(0) {}

    [[nodiscard]] ActionResult add(std::shared_ptr<Item> item) override;

    std::shared_ptr<Item> remove(std::shared_ptr<Item> item) override;

    [[nodiscard]] const std::vector<std::shared_ptr<Item>>& get_items() const override { return items_; }

    [[nodiscard]] std::shared_ptr<Item> get_item(std::size_t idx) const override;

    [[nodiscard]] int get_weight() const override { return weight_; }
    [[nodiscard]] int get_capacity() const override { return capacity_; }
    [[nodiscard]] bool can_add(std::shared_ptr<Item> item) const override;

protected:
    int capacity_ = 0;
    int weight_ = 0;
    std::vector<std::shared_ptr<Item>> items_;
};

} // namespace game

#endif //INC_3_INVENTORYCOMPONENT_H