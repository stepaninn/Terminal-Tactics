#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/entity/entities/items/Item.h"
#include "model/entity/entities/items/Weapon.h"

namespace game {

TEST_CASE("Item stores id and weight") {
  Item item(7, 42);
  REQUIRE(item.get_weight() == 7);
  REQUIRE(item.get_id() == 42);
  item.set_id(99);
  REQUIRE(item.get_id() == 99);
}

TEST_CASE("Medkit exposes heal and cost") {
  Medkit kit(5, 2, 1, 10);
  REQUIRE(kit.get_heal() == 5);
  REQUIRE(kit.get_cost() == 2);
  REQUIRE(kit.get_weight() == 1);
  REQUIRE(kit.get_id() == 10);
}

TEST_CASE("AmmoBag clamps add/reduce ammo") {
  AmmoBag bag(3, 10, AmmoType::PISTOL, 2, 11);
  REQUIRE(bag.get_current_ammo() == 3);
  REQUIRE(bag.get_max_ammo() == 10);
  REQUIRE(bag.get_ammo_type() == AmmoType::PISTOL);

  REQUIRE(bag.add_ammo(4) == 4);
  REQUIRE(bag.get_current_ammo() == 7);
  REQUIRE(bag.add_ammo(10) == 3);
  REQUIRE(bag.get_current_ammo() == 10);

  REQUIRE(bag.reduce_ammo(6) == 6);
  REQUIRE(bag.get_current_ammo() == 4);
  REQUIRE(bag.reduce_ammo(10) == 4);
  REQUIRE(bag.get_current_ammo() == 0);
}

TEST_CASE("Weapon exposes stats and clamps range/ammo") {
  Weapon weapon(Damage{2, 5}, 3, 1, 2, AmmoType::RIFLE, 4, 9, 6, 17);
  REQUIRE(weapon.get_dmg().min_dmg == 2);
  REQUIRE(weapon.get_dmg().max_dmg == 5);
  REQUIRE(weapon.get_range() == 3);
  REQUIRE(weapon.get_attack_cost() == 1);
  REQUIRE(weapon.get_reload_cost() == 2);
  REQUIRE(weapon.get_ammo_type() == AmmoType::RIFLE);
  REQUIRE(weapon.get_current_ammo() == 4);
  REQUIRE(weapon.get_max_ammo() == 9);
  REQUIRE(weapon.get_weight() == 6);
  REQUIRE(weapon.get_id() == 17);

  weapon.set_range(0);
  REQUIRE(weapon.get_range() == 1);
  weapon.set_range(7);
  REQUIRE(weapon.get_range() == 7);

  REQUIRE(weapon.add_ammo(3) == 3);
  REQUIRE(weapon.get_current_ammo() == 7);
  REQUIRE(weapon.add_ammo(10) == 2);
  REQUIRE(weapon.get_current_ammo() == 9);

  REQUIRE(weapon.reduce_ammo(4) == 4);
  REQUIRE(weapon.get_current_ammo() == 5);
  REQUIRE(weapon.reduce_ammo(10) == 5);
  REQUIRE(weapon.get_current_ammo() == 0);
}

} // namespace game
