#ifndef INC_3_INVENTORYCOMPONENT_H
#define INC_3_INVENTORYCOMPONENT_H

#include "IComponent.h"
#include "../../../types.h"
#include "../entities/items/Item.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace game {

class InventoryComponent : public IComponent {
public:
    virtual void add(std::unique_ptr<Item> item, id_t id) = 0;
    virtual std::unique_ptr<Item> remove_by_id(id_t id) = 0;

    [[nodiscard]] virtual size_t size() const noexcept = 0;
    [[nodiscard]] virtual Item* get_item(id_t id) noexcept = 0;

    [[nodiscard]] virtual int get_weight() const noexcept = 0;
    [[nodiscard]] virtual int get_capacity() const noexcept = 0;
    [[nodiscard]] virtual int get_max_weight() const noexcept = 0;
    [[nodiscard]] virtual bool can_add(const Item& item) const noexcept = 0;

    [[nodiscard]] virtual std::vector<const Item*> get_items() const = 0;

    ~InventoryComponent() override = default;
};

class DefaultInventoryComp : public InventoryComponent {
public:
    explicit DefaultInventoryComp(int capacity, int max_weight) : capacity_(capacity), max_weight_(max_weight) {}

    void add(std::unique_ptr<Item> item, id_t id) override;
    std::unique_ptr<Item> remove_by_id(id_t id) override;

    [[nodiscard]] size_t size() const noexcept override { return items_.size(); }
    [[nodiscard]] Item* get_item(id_t id) noexcept override { return items_[id].get(); }

    [[nodiscard]] int get_weight() const noexcept override { return weight_; }
    [[nodiscard]] int get_capacity() const noexcept override { return capacity_; }
    [[nodiscard]] int get_max_weight() const noexcept override { return max_weight_; }
    [[nodiscard]] bool can_add(const Item& item) const noexcept override;

    [[nodiscard]] std::vector<const Item*> get_items() const override;

protected:
    int capacity_ = 0;
    int weight_ = 0;
    int max_weight_ = 0;
    std::unordered_map<id_t, std::unique_ptr<Item>> items_;
};

} // namespace game

#endif //INC_3_INVENTORYCOMPONENT_H