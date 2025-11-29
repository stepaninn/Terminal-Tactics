#ifndef INC_3_ITEMS_H
#define INC_3_ITEMS_H

#include "types.h"
#include <memory>
#include <string>

namespace game {

class Item {
public:
    using Ptr = std::shared_ptr<Item>;
    Item() = default;
    explicit Item(int weight) : weight_(weight) {}
    virtual ~Item() = default;

    virtual int get_weight() const noexcept { return weight_; }

private:
    int weight_ = 0;
};

class AmmoBag : public Item {
public:
    using Ptr = std::shared_ptr<AmmoBag>;
    AmmoBag(int current_ammo, int max_ammo, AmmoType t, int weight = 0) :
        Item(weight), current_ammo_(current_ammo), max_ammo_(max_ammo), ammo_type_(t) {}

    int get_current_ammo() const noexcept { return current_ammo_; }
    int get_max_ammo() const noexcept { return max_ammo_; }
    AmmoType get_ammo_type() const noexcept { return ammo_type_; }

    // добавляет ammo, возвращает реальное число добавленных патронов
    int add_ammo(int ammo);

    // извлекает ammo, возвращает реальное число извлеченных патронов
    int extract_ammo(int ammo);

private:
    int current_ammo_ = 0;
    int max_ammo_ = 0;
    AmmoType ammo_type_ = AmmoType::PISTOL;
};

class Medkit : public Item {
public:
    using Ptr = std::shared_ptr<Medkit>;
    Medkit(int heal_hp, int time_cost, int weight = 0)
      : Item(weight), heal_hp_(heal_hp), time_cost_(time_cost) {}

    int get_heal() const noexcept { return heal_hp_; }
    int get_cost() const noexcept { return time_cost_; }

private:
    int heal_hp_ = 0;
    int time_cost_ = 0;
};

} // namespace game

#endif //INC_3_ITEMS_H