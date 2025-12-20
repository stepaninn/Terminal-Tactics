#ifndef MYGAMEPROJECT_GLASS_H
#define MYGAMEPROJECT_GLASS_H

#include "Cell.h"

namespace game {

class Glass final : public ICell  {
public:
    Glass() = default;

    [[nodiscard]] bool is_walkable() const noexcept override { return broken_; }
    [[nodiscard]] bool is_blocks_vision() const noexcept override { return false; }
    [[nodiscard]] bool can_shoot_through() const noexcept override { return broken_; }
    [[nodiscard]] bool apply_shot() noexcept override {
        if (broken_) return false;
        broken_ = true;
        return true;
    }

    [[nodiscard]] bool can_place_items() const noexcept override { return broken_; }

    [[nodiscard]] std::string_view view_name() const noexcept override { return "Glass"; }

    void break_glass() noexcept { broken_ = true; }
    [[nodiscard]] bool is_broken() const noexcept { return broken_; }

private:
    bool broken_ = false;
};

}

#endif //MYGAMEPROJECT_GLASS_H
