#ifndef INC_3_INVENTORYCOMPONENT_H
#define INC_3_INVENTORYCOMPONENT_H

#include "IComponent.h"
#include "../../../types.h"
#include "../entities/items/Item.h"
#include <vector>
#include <memory>

namespace game {

class InventoryComponent : public IComponent {
public:
    virtual void add(std::unique_ptr<Item> item) = 0;
    virtual std::unique_ptr<Item> remove(const Item* item) = 0;
    virtual std::unique_ptr<Item> remove_at(size_t idx) = 0;

    [[nodiscard]] virtual size_t size() const noexcept = 0;
    [[nodiscard]] virtual const Item* get_item(std::size_t idx) const = 0;
    [[nodiscard]] virtual Item* get_item(size_t idx) = 0;

    [[nodiscard]] virtual int get_weight() const = 0;
    [[nodiscard]] virtual int get_capacity() const = 0;
    [[nodiscard]] virtual bool can_add(const Item& item) const = 0;

    [[nodiscard]] virtual std::vector<const Item*> get_items() const = 0;

    ~InventoryComponent() override = default;
};

class DefaultInventoryComp : public InventoryComponent {
public:
    explicit DefaultInventoryComp(const int capacity = 0) : capacity_(capacity) {}

    void add(std::unique_ptr<Item> item) override;
    std::unique_ptr<Item> remove(const Item* item) override;
    std::unique_ptr<Item> remove_at(size_t idx) override;

    [[nodiscard]] size_t size() const noexcept override { return items_.size(); }
    [[nodiscard]] Item* get_item(size_t idx) override { return items_.at(idx).get(); }
    [[nodiscard]] const Item* get_item(size_t idx) const override { return items_.at(idx).get(); }

    [[nodiscard]] int get_weight() const override { return weight_; }
    [[nodiscard]] int get_capacity() const override { return capacity_; }
    [[nodiscard]] bool can_add(const Item& item) const override { return item.get_weight() + weight_ <= capacity_; }

    [[nodiscard]] std::vector<const Item*> get_items() const override;

protected:
    int capacity_ = 0;
    int weight_ = 0;
    std::vector<std::unique_ptr<Item>> items_;
};

} // namespace game

#endif //INC_3_INVENTORYCOMPONENT_H