#ifndef MYGAMEPROJECT_ITEMS_H
#define MYGAMEPROJECT_ITEMS_H

#include "../../../../types.h"

#include <algorithm>

namespace game::service { struct UseContext; }

namespace game::entity::items {

/// @brief Базовый класс предмета
class Item {
public:
    Item() = default;
    explicit Item(game::ItemId id, int weight) : id_(id), weight_(std::max(0, weight)) {}
    virtual ~Item() = default;

    /**
     * @brief Метод получения веса предмета
     * @return int вес предмета
     */
    [[nodiscard]] virtual int get_weight() const noexcept { return weight_; }
    /**
     * @brief Метод получения идентификатора предмета
     * @return ItemId идентификатор предмета
     */
    [[nodiscard]] virtual game::ItemId get_id() const noexcept { return id_; }
    /**
     * @brief Метод задания идентификатора предмета
     * @param id новый идентификатор
     */
    virtual void set_id(game::ItemId id) noexcept { id_ = id; }

    /**
     * @brief Метод использования предмета с учетом контекста
     * @param ctx контекст для использования предмета
     * @return bool true, если использование успешно
     */
    [[nodiscard]] virtual bool use(game::service::UseContext&) noexcept { return false; }

protected:
    game::ItemId id_{};
    int weight_ = 0;
};

/// @brief Класс подсумка для патронов
class AmmoBag final : public Item {
public:
    AmmoBag(game::ItemId id, int weight, int current_ammo, int max_ammo, game::AmmoType t)
        : Item(id, weight),
          current_ammo_(std::max(0, current_ammo)),
          max_ammo_(std::max(0, max_ammo)),
          ammo_type_(t)
    {
        if (current_ammo_ > max_ammo_) current_ammo_ = max_ammo_;
    }

    /**
     * @brief Метод получения текущего количества патронов
     * @return int текущее количество патронов
     */
    [[nodiscard]] int get_current_ammo() const noexcept { return current_ammo_; }
    /**
     * @brief Метод получения максимального количества патронов
     * @return int максимальное количество патронов
     */
    [[nodiscard]] int get_max_ammo() const noexcept { return max_ammo_; }
    /**
     * @brief Метод получения типа патронов
     * @return AmmoType тип патронов
     */
    [[nodiscard]] game::AmmoType get_ammo_type() const noexcept { return ammo_type_; }

    // сделал не сеттеры для удобства
    // добавляет ammo, возвращает реальное число добавленных патронов
    /**
     * @brief Метод добавления патронов
     * @param ammo количество добавляемых патронов
     * @return int количество реально добавленных патронов
     * @note Не добавляет ничего при ammo <= 0
     */
    [[nodiscard]] int add_ammo(int ammo);

    // извлекает ammo, возвращает реальное число извлеченных патронов
    /**
     * @brief Метод извлечения патронов
     * @param ammo количество извлекаемых патронов
     * @return int количество реально извлеченных патронов
     * @note Не извлекает ничего при ammo <= 0
     */
    [[nodiscard]] int reduce_ammo(int ammo);

    [[nodiscard]] bool use(game::service::UseContext& ctx) noexcept override;

private:
    int current_ammo_ = 0;
    int max_ammo_ = 0;
    game::AmmoType ammo_type_ = game::AmmoType::PISTOL;
};

/// @brief Класс аптечки
class Medkit final : public Item {
public:
    Medkit(game::ItemId id, int weight, int heal_hp, int time_cost)
      : Item(id, weight), heal_hp_(heal_hp), time_cost_(time_cost) {}

    /**
     * @brief Метод получения количества лечащего HP
     * @return int количество лечащего HP
     */
    [[nodiscard]] int get_heal() const noexcept { return heal_hp_; }
    /**
     * @brief Метод получения стоимости по времени
     * @return int стоимость по времени
     */
    [[nodiscard]] int get_cost() const noexcept { return time_cost_; }

    [[nodiscard]] bool use(game::service::UseContext& ctx) noexcept override;

private:
    int heal_hp_ = 0;
    int time_cost_ = 0;
};

}

#endif //MYGAMEPROJECT_ITEMS_H
