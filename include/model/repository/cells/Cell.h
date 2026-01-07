#ifndef MYGAMEPROJECT_CELL_H
#define MYGAMEPROJECT_CELL_H

#include "model/entity/entities/items/Item.h"
#include "types.h"
#include <vector>
#include <memory>
#include <unordered_map>
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
     * @brief Метод проверки возможности размещения предметов
     * @return bool true, если можно размещать предметы
     */
    [[nodiscard]] virtual bool can_place_items() const noexcept { return false; }
    /**
     * @brief Метод получения предмета по идентификатору
     * @param id идентификатор предмета
     * @return const Item* указатель на предмет или nullptr
     */
    [[nodiscard]] virtual const game::entity::items::Item* get_item(game::ItemId id) const { return nullptr; }
    /**
     * @brief Метод получения всех предметов
     * @return std::vector<const Item*> массив указателей на предметы
     */
    [[nodiscard]] virtual std::vector<const game::entity::items::Item*> get_items() const { return {}; }

    /**
     * @brief Метод получения отображаемого имени клетки
     * @return std::string_view имя клетки
     */
    [[nodiscard]] virtual std::string_view view_name() const noexcept = 0;

    /**
     * @brief Метод добавления предмета
     * @param item предмет
     */
    virtual void add([[maybe_unused]] std::unique_ptr<game::entity::items::Item> item) {}
    /**
     * @brief Метод удаления предмета по идентификатору
     * @param id идентификатор предмета
     * @return unique_ptr на удаленный предмет или nullptr
     */
    virtual std::unique_ptr<game::entity::items::Item> remove_by_id([[maybe_unused]] game::ItemId id) {
        return nullptr;
    }

    /**
     * @brief Метод получения количества предметов
     * @return size_t количество предметов
     */
    [[nodiscard]] virtual size_t size() const noexcept { return 0; }
    
};

/// @brief Хранилище предметов для клетки
struct ItemStorage {
    std::unordered_map<game::ItemId, std::unique_ptr<game::entity::items::Item>> items_;

    /**
     * @brief Метод добавления предмета
     * @param item предмет
     */
    void add(std::unique_ptr<game::entity::items::Item> item) {
        if (!item) return;
        items_[item->get_id()] = std::move(item);
    }

    /**
     * @brief Метод удаления предмета по идентификатору
     * @param id идентификатор предмета
     * @return unique_ptr на удаленный предмет или nullptr
     */
    std::unique_ptr<game::entity::items::Item> remove_by_id(game::ItemId id);

    /**
     * @brief Метод получения количества предметов
     * @return size_t количество предметов
     */
    size_t size() const noexcept { return items_.size(); }

    /**
     * @brief Метод получения всех предметов
     * @return std::vector<const Item*> массив указателей на предметы
     */
    std::vector<const game::entity::items::Item*> get_items() const;

    /**
     * @brief Метод получения предмета по идентификатору
     * @param id идентификатор предмета
     * @return const Item* указатель на предмет или nullptr
     */
    const game::entity::items::Item* get_item(game::ItemId id) const noexcept;
};

}


#endif //MYGAMEPROJECT_CELL_H
