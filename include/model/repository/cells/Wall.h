#ifndef MYGAMEPROJECT_WALL_H
#define MYGAMEPROJECT_WALL_H

#include "Cell.h"

namespace game::repo::cells {

/// @brief Класс стены
class Wall final : public ICell {
public:
    Wall() = default;

    /**
     * @brief Метод проверки проходимости
     * @return bool всегда false
     */
    [[nodiscard]] bool is_walkable() const noexcept override { return false; }
    /**
     * @brief Метод проверки блокировки обзора
     * @return bool всегда true
     */
    [[nodiscard]] bool is_blocks_vision() const noexcept override { return true; }
    /**
     * @brief Метод проверки прострела
     * @return bool всегда false
     */
    [[nodiscard]] bool can_shoot_through() const noexcept override { return false; }

    /**
     * @brief Метод получения отображаемого имени клетки
     * @return std::string_view имя клетки
     */
    [[nodiscard]] std::string_view view_name() const noexcept override { return "Wall"; }

};

}

#endif //MYGAMEPROJECT_WALL_H
