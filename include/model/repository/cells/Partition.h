#ifndef MYGAMEPROJECT_PARTITION_H
#define MYGAMEPROJECT_PARTITION_H

#include "Cell.h"

namespace game::repo::cells {

class Partition final : public ICell {
public:
    Partition() = default;

    [[nodiscard]] bool is_walkable() const noexcept override { return broken_; }
    [[nodiscard]] bool is_blocks_vision() const noexcept override { return !broken_; }
    [[nodiscard]] bool can_shoot_through() const noexcept override { return broken_; }
    [[nodiscard]] bool apply_shot() noexcept override {
        if (broken_) return false;
        broken_ = true;
        return true;
    }

    [[nodiscard]] std::string_view view_name() const noexcept override { return "Partition"; }

    void break_partition() noexcept { broken_ = true; }
    [[nodiscard]] bool is_broken() const noexcept { return broken_; }

private:
    bool broken_ = false;
};

}

#endif // MYGAMEPROJECT_PARTITION_H
