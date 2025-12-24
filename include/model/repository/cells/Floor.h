#ifndef MYGAMEPROJECT_FLOOR_H
#define MYGAMEPROJECT_FLOOR_H
#include "Cell.h"

namespace game::repo::cells {

class Floor : public ICell {
public:
    Floor() = default;

    [[nodiscard]] bool is_walkable() const noexcept override { return true; }
    [[nodiscard]] bool is_blocks_vision() const noexcept override { return false; }
    [[nodiscard]] bool can_shoot_through() const noexcept override { return true; }

    [[nodiscard]] bool can_place_items() const noexcept override { return true; }
    [[nodiscard]] const game::entity::items::Item* get_item(game::ItemId id) const override {
        return items_.get_item(id);
    }
    [[nodiscard]] std::vector<const game::entity::items::Item*> get_items() const override {
        return items_.get_items();
    }

    [[nodiscard]] std::string_view view_name() const noexcept override { return "Floor"; }

    void add(std::unique_ptr<game::entity::items::Item> item, game::ItemId id) override {
        items_.add(std::move(item), id);
    }
    std::unique_ptr<game::entity::items::Item> remove_by_id(game::ItemId id) override {
        return items_.remove_by_id(id);
    }

    [[nodiscard]] size_t size() const noexcept override { return items_.size(); }

protected:
    ItemStorage items_;
};

}

#endif //MYGAMEPROJECT_FLOOR_H
