#ifndef MYGAMEPROJECT_ITEMCONTAINER_MT_H
#define MYGAMEPROJECT_ITEMCONTAINER_MT_H

#include "model/entity/entities/items/Item_mt.h"

#include <memory>
#include <tbb/concurrent_hash_map.h>
#include <vector>

namespace game::mt::repo::cells {

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
    [[nodiscard]] virtual std::shared_ptr<const game::mt::entity::items::Item> get_item(game::mt::ItemId id) const = 0;
    /**
     * @brief Метод получения всех предметов
     * @return std::vector<const Item*> массив указателей на предметы
     */
    [[nodiscard]] virtual std::vector<std::shared_ptr<const game::mt::entity::items::Item>> get_items() const = 0;

    /**
     * @brief Метод добавления предмета
     * @param item предмет
     * @return unique_ptr на предмет при неуспехе, либо nullptr при успехе
     */
    virtual std::shared_ptr<game::mt::entity::items::Item> add(
        std::shared_ptr<game::mt::entity::items::Item> item) = 0;
    /**
     * @brief Метод удаления предмета по идентификатору
     * @param id идентификатор предмета
     * @return unique_ptr на удаленный предмет или nullptr
     */
    virtual std::shared_ptr<game::mt::entity::items::Item> remove_by_id(game::mt::ItemId id) = 0;

    /**
     * @brief Метод получения количества предметов
     * @return size_t количество предметов
     */
    [[nodiscard]] virtual size_t size() const noexcept = 0;
};

/// @brief Хранилище предметов для клетки
struct ItemStorage {
    tbb::concurrent_hash_map<game::mt::ItemId, std::shared_ptr<game::mt::entity::items::Item>> items_;

    /**
     * @brief Метод добавления предмета
     * @param item предмет
     */
    void add(std::shared_ptr<game::mt::entity::items::Item> item) {
        if (!item) return;
        tbb::concurrent_hash_map<game::mt::ItemId, std::shared_ptr<game::mt::entity::items::Item>>::accessor acc;
        if (items_.insert(acc, item->get_id())) {
            acc->second = std::move(item);
        }
    }

    /**
     * @brief Метод удаления предмета по идентификатору
     * @param id идентификатор предмета
     * @return unique_ptr на удаленный предмет или nullptr
     */
    std::shared_ptr<game::mt::entity::items::Item> remove_by_id(game::mt::ItemId id);

    /**
     * @brief Метод получения количества предметов
     * @return size_t количество предметов
     */
    size_t size() const noexcept { return items_.size(); }

    /**
     * @brief Метод получения всех предметов
     * @return std::vector<const Item*> массив указателей на предметы
     */
    std::vector<std::shared_ptr<const game::mt::entity::items::Item>> get_items() const;

    /**
     * @brief Метод получения предмета по идентификатору
     * @param id идентификатор предмета
     * @return const Item* указатель на предмет или nullptr
     */
    std::shared_ptr<const game::mt::entity::items::Item> get_item(game::mt::ItemId id) const noexcept;
};

}

#endif //MYGAMEPROJECT_ITEMCONTAINER_MT_H
