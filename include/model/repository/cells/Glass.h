#ifndef MYGAMEPROJECT_GLASS_H
#define MYGAMEPROJECT_GLASS_H

#include "Cell.h"

namespace game::repo::cells {

/// @brief Класс стеклянной клетки
class Glass final : public ICell  {
public:
    Glass() = default;

    /**
     * @brief Метод проверки проходимости
     * @return bool true, если стекло разбито
     */
    [[nodiscard]] bool is_walkable() const noexcept override { return broken_; }
    /**
     * @brief Метод проверки блокировки обзора
     * @return bool всегда false
     */
    [[nodiscard]] bool is_blocks_vision() const noexcept override { return false; }
    /**
     * @brief Метод проверки прострела
     * @return bool true, если стекло разбито
     */
    [[nodiscard]] bool can_shoot_through() const noexcept override { return broken_; }
    /**
     * @brief Метод применения выстрела по стеклу
     * @return bool true, если стекло было целым и стало разбитым
     */
    [[nodiscard]] bool apply_shot() noexcept override {
        if (broken_) return false;
        broken_ = true;
        return true;
    }

    /**
     * @brief Метод проверки возможности размещения предметов
     * @return bool true, если стекло разбито
     */
    [[nodiscard]] bool can_place_items() const noexcept override { return broken_; }

    /**
     * @brief Метод получения отображаемого имени клетки
     * @return std::string_view имя клетки
     */
    [[nodiscard]] std::string_view view_name() const noexcept override { return "Glass"; }

    /**
     * @brief Метод разбития стекла
     */
    void break_glass() noexcept { broken_ = true; }
    /**
     * @brief Метод проверки разбитости стекла
     * @return bool true, если стекло разбито
     */
    [[nodiscard]] bool is_broken() const noexcept { return broken_; }

private:
    bool broken_ = false;
};

}

#endif //MYGAMEPROJECT_GLASS_H
