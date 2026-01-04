#ifndef MYGAMEPROJECT_PARTITION_H
#define MYGAMEPROJECT_PARTITION_H

#include "Cell.h"

namespace game::repo::cells {

/// @brief Класс перегородки
class Partition final : public ICell {
public:
    Partition() = default;

    /**
     * @brief Метод проверки проходимости
     * @return bool true, если перегородка сломана
     */
    [[nodiscard]] bool is_walkable() const noexcept override { return broken_; }
    /**
     * @brief Метод проверки блокировки обзора
     * @return bool true, если перегородка целая
     */
    [[nodiscard]] bool is_blocks_vision() const noexcept override { return !broken_; }
    /**
     * @brief Метод проверки прострела
     * @return bool true, если перегородка сломана
     */
    [[nodiscard]] bool can_shoot_through() const noexcept override { return broken_; }
    /**
     * @brief Метод применения выстрела по перегородке
     * @return bool true, если перегородка была целой и стала сломанной
     */
    [[nodiscard]] bool apply_shot() noexcept override {
        if (broken_) return false;
        broken_ = true;
        return true;
    }

    /**
     * @brief Метод получения отображаемого имени клетки
     * @return std::string_view имя клетки
     */
    [[nodiscard]] std::string_view view_name() const noexcept override { return "Partition"; }

    /**
     * @brief Метод ломания перегородки
     */
    void break_partition() noexcept { broken_ = true; }
    /**
     * @brief Метод проверки сломанности перегородки
     * @return bool true, если перегородка сломана
     */
    [[nodiscard]] bool is_broken() const noexcept { return broken_; }

private:
    bool broken_ = false;
};

}

#endif // MYGAMEPROJECT_PARTITION_H
