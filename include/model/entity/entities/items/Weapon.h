#ifndef MYGAMEPROJECT_WEAPON_H
#define MYGAMEPROJECT_WEAPON_H

#include "Item.h"
#include "../../../../types.h"
#include <algorithm>

namespace game::entity::items {

/// @brief Класс оружия
class Weapon : public Item {
public:
    Weapon() = default;
    Weapon(game::ItemId id, int weight, game::Damage dmg, int range, int attack_cost, int reload_cost,
           game::AmmoType type, int cur_ammo, int max_ammo) :
            Item(id, weight),
            dmg_(dmg),
            range_(std::max(1, range)),
            attack_cost_(std::max(0, attack_cost)),
            reload_cost_(std::max(0, reload_cost)),
            ammo_type_(type),
            current_ammo_(std::max(0, cur_ammo)),
            max_ammo_(std::max(0, max_ammo)) {
        if (current_ammo_ > max_ammo_) current_ammo_ = max_ammo_;
    }

    /**
     * @brief Метод получения урона
     * @return Damage структура урона
     */
    [[nodiscard]] game::Damage get_dmg() const noexcept { return dmg_; }
    /**
     * @brief Метод задания урона
     * @param d новый урон
     */
    void set_dmg(game::Damage d) noexcept { dmg_ = d; }

    /**
     * @brief Метод получения дальности
     * @return int дальность
     */
    [[nodiscard]] int get_range() const noexcept { return range_; }
    /**
     * @brief Метод задания дальности
     * @param r новая дальность
     */
    void set_range(int r) noexcept { range_ = std::max(1, r); }

    /**
     * @brief Метод получения стоимости атаки
     * @return int стоимость атаки
     */
    [[nodiscard]] int get_attack_cost() const noexcept { return attack_cost_; }
    /**
     * @brief Метод получения стоимости перезарядки
     * @return int стоимость перезарядки
     */
    [[nodiscard]] int get_reload_cost() const noexcept { return reload_cost_; }

    /**
     * @brief Метод получения текущего количества патронов
     * @return int текущее количество патронов
     */
    [[nodiscard]] int get_current_ammo() const noexcept { return current_ammo_; }

    /**
     * @brief Метод изменения количества патронов
     * @param ammo значение изменения
     * @return int фактически примененное изменение
     */
    [[nodiscard]] int add_ammo(int ammo);
    /**
     * @brief Метод уменьшения количества патронов
     * @param ammo количество для уменьшения
     * @return int фактически уменьшенное количество
     */
    [[nodiscard]] int reduce_ammo(int ammo);

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

    template <typename RNGT>
    /**
     * @brief Метод получения значения урона
     * @tparam RNGT тип генератора случайных чисел
     * @param rng генератор случайных чисел
     * @return int значение урона
     */
    [[nodiscard]] int roll_damage(RNGT& rng) const {
        std::uniform_int_distribution dist(dmg_.min_dmg, dmg_.max_dmg);
        return dist(rng);
    }

private:
    game::Damage dmg_{0, 0};
    int range_ = 0;
    int attack_cost_ = 0;
    int reload_cost_ = 0;
    game::AmmoType ammo_type_ = game::AmmoType::PISTOL;
    int current_ammo_ = 0;
    int max_ammo_ = 0;
};

}


#endif //MYGAMEPROJECT_WEAPON_H
