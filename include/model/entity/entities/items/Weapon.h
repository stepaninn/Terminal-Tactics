#ifndef INC_3_WEAPON_H
#define INC_3_WEAPON_H

#include "Item.h"
#include "../../types.h"
#include <memory>

namespace game {

class Weapon : public Item {
public:
    Weapon() = default;
    Weapon(Damage dmg, int range, int attack_cost, int reload_cost,
           AmmoType type, int cur_ammo, int max_ammo, int weight = 0) :
            Item(weight),
            dmg_(dmg),
            range_(range),
            attack_cost_(attack_cost),
            reload_cost_(reload_cost),
            ammo_type_(type),
            current_ammo_(cur_ammo),
            max_ammo_(max_ammo)
    {}

    [[nodiscard]] Damage get_dmg() const noexcept { return dmg_; }
    void set_dmg(Damage d) noexcept { dmg_ = d; }

    [[nodiscard]] int get_range() const noexcept { return range_; }
    void set_range(int r) noexcept { range_ = r; }

    [[nodiscard]] int get_attack_cost() const noexcept { return attack_cost_; }
    [[nodiscard]] int get_reload_cost() const noexcept { return reload_cost_; }

    [[nodiscard]] int get_current_ammo() const noexcept { return current_ammo_; }

    [[nodiscard]] int add_ammo(int amount);
    [[nodiscard]] int reduce_ammo(int amount);

    [[nodiscard]] int get_max_ammo() const noexcept { return max_ammo_; }

    AmmoType get_ammo_type() const noexcept { return ammo_type_; }

private:
    Damage dmg_{0, 0};
    int range_ = 0;
    int attack_cost_ = 0;
    int reload_cost_ = 0;
    AmmoType ammo_type_ = AmmoType::PISTOL;
    int current_ammo_ = 0;
    int max_ammo_ = 0;
};

} // namespace game


#endif //INC_3_WEAPON_H