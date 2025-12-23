#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/entity/components/HealthComponent.h"
#include "model/entity/components/TimePointsComponent.h"
#include "model/entity/components/MoveComponent.h"
#include "model/entity/components/VisionComponent.h"
#include "model/entity/components/CombatComponent.h"
#include "model/entity/components/AIComponent.h"
#include "model/entity/components/PositionComponent.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/components/WeaponComponent.h"
#include "model/entity/entities/items/Item.h"
#include "model/entity/entities/items/Weapon.h"

#include <memory>
#include <vector>

namespace game {

TEST_CASE("DefaultHealthComp clamps add/reduce") {
  DefaultHealthComp hc(5, 10);
  REQUIRE(hc.add_hp(3) == 3);
  REQUIRE(hc.get_current_hp() == 8);
  REQUIRE(hc.add_hp(10) == 2);
  REQUIRE(hc.get_current_hp() == 10);
  REQUIRE(hc.reduce_hp(4) == 4);
  REQUIRE(hc.get_current_hp() == 6);
  REQUIRE(hc.reduce_hp(10) == 6);
  REQUIRE(hc.get_current_hp() == 0);
}

TEST_CASE("DefaultTimePointsComp clamps add/reduce") {
  DefaultTimePointsComp tp(2, 5);
  REQUIRE(tp.add_points(2) == 2);
  REQUIRE(tp.get_current_points() == 4);
  REQUIRE(tp.add_points(10) == 1);
  REQUIRE(tp.get_current_points() == 5);
  REQUIRE(tp.reduce_points(3) == 3);
  REQUIRE(tp.get_current_points() == 2);
  REQUIRE(tp.reduce_points(5) == 2);
  REQUIRE(tp.get_current_points() == 0);
}

TEST_CASE("DefaultMoveComp enforces min step cost") {
  DefaultMoveComp mv;
  REQUIRE(mv.get_step_cost() == 1);
  mv.set_step_cost(0);
  REQUIRE(mv.get_step_cost() == 1);
  mv.set_step_cost(5);
  REQUIRE(mv.get_step_cost() == 5);
}

TEST_CASE("DefaultVisionComp clamps radius and visibility") {
  DefaultVisionComp vc(-3, true);
  REQUIRE(vc.get_vision_radius() == 0);
  REQUIRE_FALSE(vc.is_sees_creatures());
  vc.set_vision_radius(-1);
  REQUIRE(vc.get_vision_radius() == 0);
  vc.set_vision_radius(4);
  REQUIRE(vc.get_vision_radius() == 4);
}

TEST_CASE("DefaultCombatComp clamps accuracy") {
  DefaultCombatComp cc(-2);
  REQUIRE(cc.get_base_accuracy() == 0);
  cc.set_base_accuracy(-5);
  REQUIRE(cc.get_base_accuracy() == 0);
  cc.set_base_accuracy(7);
  REQUIRE(cc.get_base_accuracy() == 7);
}

TEST_CASE("DefaultAIComp stores state") {
  DefaultAIComp ai;
  REQUIRE(ai.get_state() == AIState::NEUTRAL);
  ai.set_state(AIState::AGGRESSIVE);
  REQUIRE(ai.get_state() == AIState::AGGRESSIVE);
}

TEST_CASE("DefaultPositionComp stores position") {
  DefaultPositionComp pc;
  REQUIRE(pc.get_position().x == 0);
  REQUIRE(pc.get_position().y == 0);
  pc.set_position(Position{2, 3});
  REQUIRE(pc.get_position().x == 2);
  REQUIRE(pc.get_position().y == 3);
}

TEST_CASE("DefaultInventoryComp add/remove behavior") {
  DefaultInventoryComp inv(2, 10);
  REQUIRE(inv.size() == 0);
  REQUIRE(inv.get_weight() == 0);

  auto medkit = std::make_unique<Medkit>(11, 1, 3, 5);
  auto ammo = std::make_unique<AmmoBag>(12, 4, 5, 10, AmmoType::PISTOL);

  REQUIRE(inv.can_add(*medkit));
  inv.add(std::move(medkit), 11);
  inv.add(std::move(ammo), 12);

  REQUIRE(inv.size() == 2);
  REQUIRE(inv.get_weight() == 5);
  REQUIRE(inv.get_item(11) != nullptr);
  REQUIRE(inv.get_item(12) != nullptr);

  auto too_many = std::make_unique<Medkit>(13, 1, 1, 1);
  REQUIRE_THROWS_AS(inv.add(std::move(too_many), 13), std::runtime_error);

  auto removed = inv.remove_by_id(11);
  REQUIRE(removed != nullptr);
  REQUIRE(removed->get_id() == 11);
  REQUIRE(inv.size() == 1);
  REQUIRE(inv.get_weight() == 4);
  REQUIRE(inv.get_item(11) == nullptr);

  REQUIRE(inv.remove_by_id(99) == nullptr);

  auto items = inv.get_items();
  REQUIRE(items.size() == 1);
  REQUIRE(items[0]->get_id() == 12);
}

TEST_CASE("DefaultWeaponComp returns previous weapon") {
  auto w1 = std::make_shared<Weapon>(1, 2, Damage{1, 2}, 3, 1, 2, AmmoType::PISTOL, 1, 5);
  auto w2 = std::make_shared<Weapon>(2, 3, Damage{2, 3}, 4, 2, 3, AmmoType::RIFLE, 2, 6);

  DefaultWeaponComp wc(w1);
  REQUIRE(wc.get_weapon() == w1);
  auto prev = wc.set_weapon(w2);
  REQUIRE(prev == w1);
  REQUIRE(wc.get_weapon() == w2);
}

} // namespace game
