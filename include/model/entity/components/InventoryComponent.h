#ifndef INC_3_INVENTORYCOMPONENT_H
#define INC_3_INVENTORYCOMPONENT_H

#include "IComponent.h"
#include "../../../types.h"
#include "../entities/items/Item.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace game::entity::components {

class InventoryComponent : public IComponent {
public:
    virtual void add(std::unique_ptr<game::entity::items::Item> item, game::id_t id) = 0;
    virtual std::unique_ptr<game::entity::items::Item> remove_by_id(game::id_t id) = 0;

    [[nodiscard]] virtual size_t size() const noexcept = 0;
    [[nodiscard]] virtual const game::entity::items::Item* get_item(game::id_t id) const noexcept = 0;

    [[nodiscard]] virtual int get_weight() const noexcept = 0;
    [[nodiscard]] virtual int get_capacity() const noexcept = 0;
    [[nodiscard]] virtual int get_max_weight() const noexcept = 0;
    [[nodiscard]] virtual bool can_add(const game::entity::items::Item& item) const noexcept = 0;

    [[nodiscard]] virtual std::vector<const game::entity::items::Item*> get_items() const = 0;

    ~InventoryComponent() override = default;
};

class DefaultInventoryComp : public InventoryComponent {
public:
    explicit DefaultInventoryComp(int capacity, int max_weight)
        : capacity_(std::max(0, capacity)), max_weight_(std::max(0, max_weight)) {}

    void add(std::unique_ptr<game::entity::items::Item> item, game::id_t id) override;
    std::unique_ptr<game::entity::items::Item> remove_by_id(game::id_t id) override;

    [[nodiscard]] size_t size() const noexcept override { return items_.size(); }
    [[nodiscard]] const game::entity::items::Item* get_item(game::id_t id) const noexcept override;

    [[nodiscard]] int get_weight() const noexcept override { return weight_; }
    [[nodiscard]] int get_capacity() const noexcept override { return capacity_; }
    [[nodiscard]] int get_max_weight() const noexcept override { return max_weight_; }
    [[nodiscard]] bool can_add(const game::entity::items::Item& item) const noexcept override;

    [[nodiscard]] std::vector<const game::entity::items::Item*> get_items() const override;

protected:
    int capacity_ = 0;
    int weight_ = 0;
    int max_weight_ = 0;
    std::unordered_map<game::id_t, std::unique_ptr<game::entity::items::Item>> items_;
};

}

#endif //INC_3_INVENTORYCOMPONENT_H
