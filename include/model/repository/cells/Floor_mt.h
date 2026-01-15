#ifndef MYGAMEPROJECT_FLOOR_MT_H
#define MYGAMEPROJECT_FLOOR_MT_H

#include "Cell_mt.h"
#include "ItemContainer_mt.h"

namespace game::mt::repo::cells {

/// @brief Класс клетки пола
class Floor : public ICell, public IItemContainer {
public:
    Floor() = default;

    /**
     * @brief Метод проверки проходимости
     * @return bool всегда true
     */
    [[nodiscard]] bool is_walkable() const noexcept override { return true; }
    /**
     * @brief Метод проверки блокировки обзора
     * @return bool всегда false
     */
    [[nodiscard]] bool is_blocks_vision() const noexcept override { return false; }
    /**
     * @brief Метод проверки прострела
     * @return bool всегда true
     */
    [[nodiscard]] bool can_shoot_through() const noexcept override { return true; }

    /**
     * @brief Метод проверки возможности размещения предметов
     * @return bool всегда true
     */
    [[nodiscard]] bool can_place_items() const noexcept override { return true; }
    /**
     * @brief Метод получения предмета по идентификатору
     * @param id идентификатор предмета
     * @return const Item* указатель на предмет или nullptr
     */
    [[nodiscard]] std::shared_ptr<const game::mt::entity::items::Item> get_item(game::mt::ItemId id) const override {
        return items_.get_item(id);
    }
    /**
     * @brief Метод получения всех предметов
     * @return std::vector<const Item*> массив указателей на предметы
     */
    [[nodiscard]] std::vector<std::shared_ptr<const game::mt::entity::items::Item>> get_items() const override {
        return items_.get_items();
    }

    /**
     * @brief Метод получения отображаемого имени клетки
     * @return std::string_view имя клетки
     */
    [[nodiscard]] std::string_view view_name() const noexcept override { return "Floor"; }

    /**
     * @brief Метод добавления предмета
     * @param item предмет
     * @return unique_ptr на предмет при неуспехе, либо nullptr при успехе
     */
    std::shared_ptr<game::mt::entity::items::Item> add(
        std::shared_ptr<game::mt::entity::items::Item> item) override {
        if (!item) return item;
        items_.add(std::move(item));
        return nullptr;
    }
    /**
     * @brief Метод удаления предмета по идентификатору
     * @param id идентификатор предмета
     * @return unique_ptr на удаленный предмет или nullptr
     */
    std::shared_ptr<game::mt::entity::items::Item> remove_by_id(game::mt::ItemId id) override {
        return items_.remove_by_id(id);
    }

    /**
     * @brief Метод получения количества предметов
     * @return size_t количество предметов
     */
    [[nodiscard]] size_t size() const noexcept override { return items_.size(); }

protected:
    ItemStorage items_;
};

}

#endif //MYGAMEPROJECT_FLOOR_MT_H
