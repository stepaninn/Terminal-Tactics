#ifndef MYGAMEPROJECT_GLASS_H
#define MYGAMEPROJECT_GLASS_H

#include "Cell.h"
#include "DestructibleCell.h"
#include "ItemContainer.h"

namespace game::repo::cells {

/// @brief Класс стеклянной клетки
class Glass final : public ICell, public IItemContainer, public IDestructibleCell {
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
     * @brief Метод проверки возможности разрушения выстрелом
     * @return bool true, если стекло целое
     */
    [[nodiscard]] bool can_be_shot() const noexcept override { return !broken_; }
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
     * @brief Метод получения предмета по идентификатору
     * @param id идентификатор предмета
     * @return const Item* указатель на предмет или nullptr
     */
    [[nodiscard]] const game::entity::items::Item* get_item(game::ItemId id) const override {
        return items_.get_item(id);
    }
    /**
     * @brief Метод получения всех предметов
     * @return std::vector<const Item*> массив указателей на предметы
     */
    [[nodiscard]] std::vector<const game::entity::items::Item*> get_items() const override {
        return items_.get_items();
    }

    /**
     * @brief Метод получения отображаемого имени клетки
     * @return std::string_view имя клетки
     */
    [[nodiscard]] std::string_view view_name() const noexcept override { return "Glass"; }

    /**
     * @brief Метод добавления предмета
     * @param item предмет
     * @return unique_ptr на предмет при неуспехе, либо nullptr при успехе
     */
    std::unique_ptr<game::entity::items::Item> add(
        std::unique_ptr<game::entity::items::Item> item) override {
        if (!can_place_items() || !item) return item;
        items_.add(std::move(item));
        return nullptr;
    }
    /**
     * @brief Метод удаления предмета по идентификатору
     * @param id идентификатор предмета
     * @return unique_ptr на удаленный предмет или nullptr
     */
    std::unique_ptr<game::entity::items::Item> remove_by_id(game::ItemId id) override {
        return items_.remove_by_id(id);
    }

    /**
     * @brief Метод получения количества предметов
     * @return size_t количество предметов
     */
    [[nodiscard]] size_t size() const noexcept override { return items_.size(); }

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
    ItemStorage items_;
};

}

#endif //MYGAMEPROJECT_GLASS_H
