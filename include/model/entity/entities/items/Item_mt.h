#ifndef MYGAMEPROJECT_ITEMS_MT_H
#define MYGAMEPROJECT_ITEMS_MT_H

#include "../../../../types_mt.h"

#include <algorithm>
#include <mutex>

namespace game::mt::service { struct UseContext; }

namespace game::mt::entity::items {

/// @brief Базовый класс предмета
class Item {
public:
    Item() = default;
    explicit Item(game::mt::ItemId id, int weight) : id_(id), weight_(std::max(0, weight)) {}
    virtual ~Item() = default;

    /**
     * @brief Метод получения веса предмета
     * @return int вес предмета
     */
    [[nodiscard]] virtual int get_weight() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return weight_;
    }
    /**
     * @brief Метод получения идентификатора предмета
     * @return ItemId идентификатор предмета
     */
    [[nodiscard]] virtual game::mt::ItemId get_id() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return id_;
    }
    /**
     * @brief Метод задания идентификатора предмета
     * @param id новый идентификатор
     */
    virtual void set_id(game::mt::ItemId id) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        id_ = id;
    }

    /**
     * @brief Метод использования предмета с учетом контекста
     * @param ctx контекст для использования предмета
     * @return bool true, если использование успешно
     */
    [[nodiscard]] virtual bool use(game::mt::service::UseContext&) noexcept { return false; }

protected:
    mutable std::mutex mutex_;
    game::mt::ItemId id_{};
    int weight_ = 0;
};

/// @brief Класс подсумка для патронов
class AmmoBag final : public Item {
public:
    AmmoBag(game::mt::ItemId id, int weight, int current_ammo, int max_ammo, game::mt::AmmoType t)
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
    [[nodiscard]] int get_current_ammo() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return current_ammo_;
    }
    /**
     * @brief Метод получения максимального количества патронов
     * @return int максимальное количество патронов
     */
    [[nodiscard]] int get_max_ammo() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return max_ammo_;
    }
    /**
     * @brief Метод получения типа патронов
     * @return AmmoType тип патронов
     */
    [[nodiscard]] game::mt::AmmoType get_ammo_type() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return ammo_type_;
    }

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

    [[nodiscard]] bool use(game::mt::service::UseContext& ctx) noexcept override;

private:
    mutable std::mutex mutex_;
    int current_ammo_ = 0;
    int max_ammo_ = 0;
    game::mt::AmmoType ammo_type_ = game::mt::AmmoType::PISTOL;
};

/// @brief Класс аптечки
class Medkit final : public Item {
public:
    Medkit(game::mt::ItemId id, int weight, int heal_hp, int time_cost)
      : Item(id, weight), heal_hp_(heal_hp), time_cost_(time_cost) {}

    /**
     * @brief Метод получения количества лечащего HP
     * @return int количество лечащего HP
     */
    [[nodiscard]] int get_heal() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return heal_hp_;
    }
    /**
     * @brief Метод получения стоимости по времени
     * @return int стоимость по времени
     */
    [[nodiscard]] int get_cost() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return time_cost_;
    }

    [[nodiscard]] bool use(game::mt::service::UseContext& ctx) noexcept override;

private:
    mutable std::mutex mutex_;
    int heal_hp_ = 0;
    int time_cost_ = 0;
};

}

#endif //MYGAMEPROJECT_ITEMS_MT_H
