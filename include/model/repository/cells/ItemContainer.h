#ifndef MYGAMEPROJECT_ITEMCONTAINER_H
#define MYGAMEPROJECT_ITEMCONTAINER_H

#include "model/entity/entities/items/Item.h"
#include "types.h"

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace game::repo::cells {

/// @brief Интерфейс хранения предметов в клетке
class IItemContainer {
public:
    virtual ~IItemContainer() = default;

    /**
     * @brief Метод проверки возможности размещения предметов
     * @return bool true, если можно размещать предметы
     */
    [[nodiscard]] virtual bool can_place_items() const noexcept = 0;
    /**
     * @brief Метод получения предмета по идентификатору
     * @param id идентификатор предмета
     * @return const Item* указатель на предмет или nullptr
     */
    [[nodiscard]] virtual const game::entity::items::Item* get_item(game::ItemId id) const = 0;
    /**
     * @brief Метод получения всех предметов
     * @return std::vector<const Item*> массив указателей на предметы
     */
    [[nodiscard]] virtual std::vector<const game::entity::items::Item*> get_items() const = 0;

    /**
     * @brief Метод добавления предмета
     * @param item предмет
     * @return unique_ptr на предмет при неуспехе, либо nullptr при успехе
     */
    virtual std::unique_ptr<game::entity::items::Item> add(std::unique_ptr<game::entity::items::Item> item) = 0;
    /**
     * @brief Метод удаления предмета по идентификатору
     * @param id идентификатор предмета
     * @return unique_ptr на удаленный предмет или nullptr
     */
    virtual std::unique_ptr<game::entity::items::Item> remove_by_id(game::ItemId id) = 0;

    /**
     * @brief Метод получения количества предметов
     * @return size_t количество предметов
     */
    [[nodiscard]] virtual size_t size() const noexcept = 0;
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

#endif //MYGAMEPROJECT_ITEMCONTAINER_H
