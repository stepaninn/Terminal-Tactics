#ifndef MYGAMEPROJECT_INVENTORYCOMPONENT_H
#define MYGAMEPROJECT_INVENTORYCOMPONENT_H

#include "IComponent.h"
#include "../../../types.h"
#include "../entities/items/Item.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace game::entity::components {

/// @brief Класс компонента инвентаря
class InventoryComponent : public IComponent {
public:
    /**
     * @brief Метод добавления предмет в инвентарь
     * @param item unique_ptr указатель на предмет (создается фабрикой)
     */
    virtual void add(std::unique_ptr<game::entity::items::Item> item) = 0;
    /**
     * @brief Метод удаления предмета по его ID
     * @param id ID удаляемого предмета
     * @return unique_ptr указатель на удаленный предмет. Возвращает nullptr, если такого предмета нет
     */
    virtual std::unique_ptr<game::entity::items::Item> remove_by_id(game::id_t id) = 0;

    /**
     * @brief Метод получения размера инвентаря
     * @return size_t размер инвентаря
     */
    [[nodiscard]] virtual size_t size() const noexcept = 0;
    /**
     * @brief Метод получения предмета в инвентаре для чтения
     * @param id ID желаемого предмета
     * @return Константный указатель на предмет (только для чтения)
     */
    [[nodiscard]] virtual const game::entity::items::Item* get_item(game::id_t id) const noexcept = 0;

    /**
     * @brief Метод получения предмета в инвентаре для чтения
     * @param id ID желаемого предмета
     * @return Указатель на предмет
     */
    [[nodiscard]] virtual game::entity::items::Item* get_item(game::id_t id) noexcept = 0;

    /**
     * @brief Метод получения текущего веса инвентаря
     * @return int текущий вес инвентаря
     */
    [[nodiscard]] virtual int get_weight() const noexcept = 0;
    /**
     * @brief Метод получения вместимости инвентаря
     * @return int вместимость инвентаря
     */
    [[nodiscard]] virtual int get_capacity() const noexcept = 0;
    /**
     * @brief Метод получения максимального веса инвентаря
     * @return int максимальный вес инвентаря
     */
    [[nodiscard]] virtual int get_max_weight() const noexcept = 0;
    /**
     * @brief Метод проверки добавления предмета в инвентарь
     * @param item Константная ссылка на предмет
     * @return bool возможность добавления
     */
    [[nodiscard]] virtual bool can_add(const game::entity::items::Item& item) const noexcept = 0;

    /**
     * @brief Метод получения предметов в инвентаре (аналог view)
     * @return Массив константный указатель предметов в инвентаре
     */
    [[nodiscard]] virtual std::vector<const game::entity::items::Item*> get_items() const = 0;

    ~InventoryComponent() override = default;
};

class DefaultInventoryComp : public InventoryComponent {
public:
    explicit DefaultInventoryComp(int capacity, int max_weight)
        : capacity_(std::max(0, capacity)), max_weight_(std::max(0, max_weight)) {}

    void add(std::unique_ptr<game::entity::items::Item> item) override;
    std::unique_ptr<game::entity::items::Item> remove_by_id(game::id_t id) override;

    [[nodiscard]] size_t size() const noexcept override { return items_.size(); }

    [[nodiscard]] game::entity::items::Item* get_item(game::id_t id) noexcept override;
    [[nodiscard]] const game::entity::items::Item* get_item(game::id_t id) const noexcept override;

    [[nodiscard]] int get_weight() const noexcept override { return weight_; }
    [[nodiscard]] int get_capacity() const noexcept override { return capacity_; }
    [[nodiscard]] int get_max_weight() const noexcept override { return max_weight_; }
    [[nodiscard]] bool can_add(const game::entity::items::Item& item) const noexcept override;

    [[nodiscard]] std::vector<const game::entity::items::Item*> get_items() const override;

protected:
    int capacity_ = 0;
    int weight_ = 0;
    int max_weight_ = 0;
    std::unordered_map<game::id_t, std::unique_ptr<game::entity::items::Item>> items_;
};

}

#endif //MYGAMEPROJECT_INVENTORYCOMPONENT_H
