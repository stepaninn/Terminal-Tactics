#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/repository/Level.h"
#include "model/repository/cells/Floor.h"
#include "model/entity/entities/Entity.h"

#include <memory>

namespace game {

TEST_CASE("Level spawns entities with sequential ids") {
  Level level(1, "L1");

  level.spawn_entity(std::make_unique<Entity>(), Position{0, 0});
  auto* e1 = level.get_entity(1);
  REQUIRE(e1 != nullptr);
  REQUIRE(e1->get_id() == 1);

  level.spawn_entity(std::make_unique<Entity>(), Position{1, 1});
  auto* e2 = level.get_entity(2);
  REQUIRE(e2 != nullptr);
  REQUIRE(e2->get_id() == 2);

  REQUIRE(level.get_entity(999) == nullptr);
  REQUIRE(level.get_entities().size() == 2);
}

TEST_CASE("Level cell access and replacement") {
  Level level(1, "L1");
  level.resize_field(2, 3);

  REQUIRE(level.get_cell(Position{0, 0}) == nullptr);

  auto prev = level.set_cell(Position{0, 0}, std::make_unique<Floor>());
  REQUIRE(prev == nullptr);
  REQUIRE(level.get_cell(Position{0, 0}) != nullptr);

  auto prev2 = level.set_cell(Position{0, 0}, std::make_unique<Floor>());
  REQUIRE(prev2 != nullptr);
  REQUIRE(level.get_cell(Position{0, 0}) != nullptr);

  REQUIRE(level.get_cell(Position{10, 10}) == nullptr);
  REQUIRE(level.set_cell(Position{10, 10}, std::make_unique<Floor>()) == nullptr);
}

TEST_CASE("Level remove entity by id and pointer") {
  Level level(1, "L1");

  level.spawn_entity(std::make_unique<Entity>(), Position{0, 0});
  level.spawn_entity(std::make_unique<Entity>(), Position{1, 1});

  auto* e1 = level.get_entity(1);
  auto removed1 = level.remove_entity(e1);
  REQUIRE(removed1 != nullptr);
  REQUIRE(removed1->get_id() == 1);
  REQUIRE(level.get_entity(1) == nullptr);

  auto removed2 = level.remove_entity(2);
  REQUIRE(removed2 != nullptr);
  REQUIRE(removed2->get_id() == 2);
  REQUIRE(level.get_entities().empty());

  REQUIRE(level.remove_entity(999) == nullptr);
}

TEST_CASE("Level entity radius query") {
  Level level(1, "L1");

  level.spawn_entity(std::make_unique<Entity>(), Position{0, 0});
  level.spawn_entity(std::make_unique<Entity>(), Position{3, 4});

  auto near = level.get_entities_radius(Position{0, 0}, 4);
  REQUIRE(near.size() == 1);

  auto all = level.get_entities_radius(Position{0, 0}, 5);
  REQUIRE(all.size() == 2);

  auto none = level.get_entities_radius(Position{0, 0}, -1);
  REQUIRE(none.empty());
}

} // namespace game
