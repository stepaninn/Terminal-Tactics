#ifndef INC_3_ITEMS_H
#define INC_3_ITEMS_H

#include "../../../../types.h"

namespace game {

class Item {
public:
    Item() = default;
    explicit Item(int weight, id_t id = {}) : id_(id), weight_(weight) {}
    virtual ~Item() = default;

    [[nodiscard]] virtual int get_weight() const noexcept { return weight_; }
    [[nodiscard]] virtual id_t get_id() const noexcept { return id_; }
    virtual void set_id(id_t id) noexcept { id_ = id; }

protected:
    id_t id_{};
    int weight_ = 0;
};

class AmmoBag : public Item {
public:
    AmmoBag(int current_ammo, int max_ammo, AmmoType t, int weight = 0, id_t id = {}) :
        Item(weight, id), current_ammo_(current_ammo), max_ammo_(max_ammo), ammo_type_(t) {}

    [[nodiscard]] int get_current_ammo() const noexcept { return current_ammo_; }
    [[nodiscard]] int get_max_ammo() const noexcept { return max_ammo_; }
    [[nodiscard]] AmmoType get_ammo_type() const noexcept { return ammo_type_; }

    // сделал не сеттеры для удобства
    // добавляет ammo, возвращает реальное число добавленных патронов
    [[nodiscard]] int add_ammo(int ammo);

    // извлекает ammo, возвращает реальное число извлеченных патронов
    [[nodiscard]] int reduce_ammo(int ammo);

private:
    int current_ammo_ = 0;
    int max_ammo_ = 0;
    AmmoType ammo_type_ = AmmoType::PISTOL;
};

class Medkit : public Item {
public:
    Medkit(int heal_hp, int time_cost, int weight = 0, id_t id = {})
      : Item(weight, id), heal_hp_(heal_hp), time_cost_(time_cost) {}

    [[nodiscard]] int get_heal() const noexcept { return heal_hp_; }
    [[nodiscard]] int get_cost() const noexcept { return time_cost_; }

private:
    int heal_hp_ = 0;
    int time_cost_ = 0;
};

} // namespace game

#endif //INC_3_ITEMS_H