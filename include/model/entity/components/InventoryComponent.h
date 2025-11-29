#ifndef INC_3_INVENTORYCOMPONENT_H
#define INC_3_INVENTORYCOMPONENT_H

#include "IComponent.h"
#include "types.h"
#include "entities/items/item.h"
#include <vector>

namespace game {

class InventoryComponent : public IComponent {
public:
    using Ptr = std::shared_ptr<InventoryComponent>;
    virtual ActionResult add(std::shared_ptr<Item> item) = 0;
    virtual std::shared_ptr<Item> remove(std::shared_ptr<Item> item) = 0;
    virtual const std::vector<std::shared_ptr<Item>>& get_items() const = 0;
    virtual std::shared_ptr<Item> get_item(std::size_t idx) const = 0;
    virtual int get_weight() const = 0;
    virtual int get_capacity() const = 0;
    virtual bool can_add(std::shared_ptr<Item> item) const = 0;
    virtual ~InventoryComponent() = default;
};

class DefaultInventoryComp : public InventoryComponent {
public:
    DefaultInventoryComp(int capacity = 0) : capacity_(capacity), weight_(0) {}

    ActionResult add(std::shared_ptr<Item> item) override;

    std::shared_ptr<Item> remove(std::shared_ptr<Item> item) override;

    const std::vector<std::shared_ptr<Item>>& get_items() const override { return items_; }

    std::shared_ptr<Item> get_item(std::size_t idx) const override;

    int get_weight() const override { return weight_; }
    int get_capacity() const override { return capacity_; }
    bool can_add(std::shared_ptr<Item> item) const override;

protected:
    int capacity_ = 0;
    int weight_ = 0;
    std::vector<std::shared_ptr<Item>> items_;
};

} // namespace game

#endif //INC_3_INVENTORYCOMPONENT_H