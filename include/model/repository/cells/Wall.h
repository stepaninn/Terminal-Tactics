#ifndef MYGAMEPROJECT_WALL_H
#define MYGAMEPROJECT_WALL_H

#include "Cell.h"

namespace game {

class Wall final : public ICell {
public:
    Wall() = default;

    [[nodiscard]] bool is_walkable() const noexcept override { return false; }
    [[nodiscard]] bool is_blocks_vision() const noexcept override { return true; }
    [[nodiscard]] bool can_shoot_through() const noexcept override { return false; }

    [[nodiscard]] std::string_view view_name() const noexcept override { return "Wall"; }

};

}

#endif //MYGAMEPROJECT_WALL_H
