#ifndef MYGAMEPROJECT_PARTITION_MT_H
#define MYGAMEPROJECT_PARTITION_MT_H

#include "Cell_mt.h"
#include "DestructibleCell_mt.h"

#include <atomic>

namespace game::mt::repo::cells {

/// @brief Класс перегородки
class Partition final : public ICell, public IDestructibleCell {
public:
    Partition() = default;

    /**
     * @brief Метод проверки проходимости
     * @return bool true, если перегородка сломана
     */
    [[nodiscard]] bool is_walkable() const noexcept override { return broken_.load(); }
    /**
     * @brief Метод проверки блокировки обзора
     * @return bool true, если перегородка целая
     */
    [[nodiscard]] bool is_blocks_vision() const noexcept override { return !broken_.load(); }
    /**
     * @brief Метод проверки прострела
     * @return bool true, если перегородка сломана
     */
    [[nodiscard]] bool can_shoot_through() const noexcept override { return broken_.load(); }
    /**
     * @brief Метод проверки возможности разрушения выстрелом
     * @return bool true, если перегородка целая
     */
    [[nodiscard]] bool can_be_shot() const noexcept override { return !broken_.load(); }
    /**
     * @brief Метод применения выстрела по перегородке
     * @return bool true, если перегородка была целой и стала сломанной
     */
    [[nodiscard]] bool apply_shot() noexcept override {
        bool expected = false;
        return broken_.compare_exchange_strong(expected, true);
    }

    /**
     * @brief Метод получения отображаемого имени клетки
     * @return std::string_view имя клетки
     */
    [[nodiscard]] std::string_view view_name() const noexcept override { return "Partition"; }

    /**
     * @brief Метод ломания перегородки
     */
    void break_partition() noexcept { broken_.store(true); }
    /**
     * @brief Метод проверки сломанности перегородки
     * @return bool true, если перегородка сломана
     */
    [[nodiscard]] bool is_broken() const noexcept { return broken_.load(); }

private:
    std::atomic<bool> broken_{false};
};

}

#endif // MYGAMEPROJECT_PARTITION_MT_H
