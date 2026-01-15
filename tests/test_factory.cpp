#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/entity/EntityFactory.h"

namespace game {

using entity::factory::EntityFactory;
using entity::components::AIBehavior;
using entity::items::Weapon;

TEST_CASE("EntityFactory creates operative with required components") {
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 3, 1, 1, AmmoType::PISTOL, 2, 4);
  auto op = EntityFactory::create_operative(10, "Op", std::move(weapon), {}, 5);

  REQUIRE(op->get_id() == 10);
  REQUIRE(op->get_name() == std::string("Op"));
  REQUIRE(op->get_team_id() == 5);
  REQUIRE(op->inventory_comp() != nullptr);
  REQUIRE(op->health_comp() != nullptr);
  REQUIRE(op->weapon_comp() != nullptr);
  REQUIRE(op->time_points_comp() != nullptr);
  REQUIRE(op->vision_comp() != nullptr);
  REQUIRE(op->combat_comp() != nullptr);
  REQUIRE(op->move_comp() != nullptr);
}

TEST_CASE("EntityFactory creates enemy types with required components and behavior") {
  auto w1 = std::make_unique<Weapon>(2, 1, Damage{1, 2}, 3, 1, 1, AmmoType::PISTOL, 2, 4);
  auto wild = EntityFactory::create_wild(1, "Wild", std::move(w1));
  REQUIRE(wild->ai_comp() != nullptr);
  REQUIRE(wild->ai_comp()->get_behavior() == AIBehavior::WILD);
  REQUIRE(wild->health_comp() != nullptr);
  REQUIRE(wild->move_comp() != nullptr);
  REQUIRE(wild->time_points_comp() != nullptr);
  REQUIRE(wild->combat_comp() != nullptr);
  REQUIRE(wild->vision_comp() != nullptr);

  auto w2 = std::make_unique<Weapon>(3, 1, Damage{1, 2}, 3, 1, 1, AmmoType::RIFLE, 2, 4);
  auto intel = EntityFactory::create_intelligent(2, "Intel", std::move(w2));
  REQUIRE(intel->ai_comp() != nullptr);
  REQUIRE(intel->ai_comp()->get_behavior() == AIBehavior::INTELLIGENT);
  REQUIRE(intel->weapon_comp() != nullptr);
  REQUIRE(intel->combat_comp() != nullptr);
  REQUIRE(intel->vision_comp() != nullptr);

  auto w3 = std::make_unique<Weapon>(4, 1, Damage{1, 2}, 3, 1, 1, AmmoType::SHOTGUN, 2, 4);
  auto forager = EntityFactory::create_forager(3, "Forager", std::move(w3));
  REQUIRE(forager->ai_comp() != nullptr);
  REQUIRE(forager->ai_comp()->get_behavior() == AIBehavior::FORAGER);
  REQUIRE(forager->inventory_comp() != nullptr);
  REQUIRE(forager->vision_comp() != nullptr);
}

}
