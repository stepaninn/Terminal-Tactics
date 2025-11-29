#ifndef INC_3_GAME_H
#define INC_3_GAME_H

#include "types.hpp"
#include <vector>
#include <memory>

namespace game {

class Missions {
public:
    Missions() = default;

    std::weak_ptr<Level> get_current_level() const noexcept;

    bool set_current_level(size_t level_index) noexcept;

    void add_level(std::shared_ptr<Level> level);

    std::size_t size() const noexcept { return missions_.size(); }

private:
    std::vector<std::shared_ptr<Level>> missions_;
    size_t current_lvl_{static_cast<size_t>(-1)}; // типа npos в строках
};

} // namespace game

#endif //INC_3_GAME_H