#ifndef MYGAMEPROJECT_CELL_H
#define MYGAMEPROJECT_CELL_H

#include <string_view>

namespace game::repo::cells {

/// @brief Интерфейс клетки уровня
class ICell {
public:
    virtual ~ICell() = default;
    /**
     * @brief Метод проверки проходимости
     * @return bool true, если по клетке можно ходить
     */
    [[nodiscard]] virtual bool is_walkable() const noexcept = 0;
    /**
     * @brief Метод проверки блокировки обзора
     * @return bool true, если клетка блокирует обзор
     */
    [[nodiscard]] virtual bool is_blocks_vision() const noexcept = 0;
    /**
     * @brief Метод проверки прострела
     * @return bool true, если можно стрелять сквозь клетку
     */
    [[nodiscard]] virtual bool can_shoot_through() const noexcept = 0;
    /**
     * @brief Метод применения выстрела по клетке
     * @return bool true, если состояние клетки изменилось
     */
    [[nodiscard]] virtual bool apply_shot() noexcept { return false; }

    /**
     * @brief Метод получения отображаемого имени клетки
     * @return std::string_view имя клетки
     */
    [[nodiscard]] virtual std::string_view view_name() const noexcept = 0;
    
};

}


#endif //MYGAMEPROJECT_CELL_H
