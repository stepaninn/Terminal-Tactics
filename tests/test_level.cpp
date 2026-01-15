#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/repository/Level.h"
#include "model/repository/cells/Floor.h"
#include "model/repository/cells/Glass.h"
#include "model/repository/cells/ItemContainer.h"
#include "model/entity/entities/Entity.h"
#include "model/entity/entities/items/Item.h"

#include <memory>

namespace game {

using entity::Entity;
using entity::items::Item;
using entity::items::Medkit;
using repo::Level;
using repo::cells::Floor;
using repo::cells::Glass;
using repo::cells::IItemContainer;

TEST_CASE("Level spawns entities with provided ids") {
  Level level(1, "L1");

  auto e1 = std::make_unique<Entity>();
  e1->set_id(10);
  level.spawn_entity(std::move(e1), Position{0, 0});
  auto* ent1 = level.get_entity(10);
  REQUIRE(ent1 != nullptr);
  REQUIRE(ent1->get_id() == 10);

  auto e2 = std::make_unique<Entity>();
  e2->set_id(42);
  level.spawn_entity(std::move(e2), Position{1, 1});
  auto* ent2 = level.get_entity(42);
  REQUIRE(ent2 != nullptr);
  REQUIRE(ent2->get_id() == 42);

  REQUIRE(level.get_entity(999) == nullptr);
  REQUIRE(level.get_entities().size() == 2);
}

TEST_CASE("Level rejects duplicate entity ids") {
  Level level(1, "L1");

  auto e1 = std::make_unique<Entity>();
  e1->set_id(7);
  level.spawn_entity(std::move(e1), Position{0, 0});

  auto e2 = std::make_unique<Entity>();
  e2->set_id(7);
  REQUIRE(level.spawn_entity(std::move(e2), Position{1, 1}) == nullptr);

  REQUIRE(level.get_entities().size() == 1);
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
  auto out_of_bounds = level.set_cell(Position{10, 10}, std::make_unique<Floor>());
  REQUIRE(out_of_bounds != nullptr);
}

TEST_CASE("Level remove entity by id and pointer") {
  Level level(1, "L1");

  auto e1 = std::make_unique<Entity>();
  e1->set_id(10);
  level.spawn_entity(std::move(e1), Position{0, 0});
  auto e2 = std::make_unique<Entity>();
  e2->set_id(11);
  level.spawn_entity(std::move(e2), Position{1, 1});

  auto* ent1 = level.get_entity(10);
  auto removed1 = level.remove_entity(ent1);
  REQUIRE(removed1 != nullptr);
  REQUIRE(removed1->get_id() == 10);
  REQUIRE(level.get_entity(10) == nullptr);

  auto removed2 = level.remove_entity(11);
  REQUIRE(removed2 != nullptr);
  REQUIRE(removed2->get_id() == 11);
  REQUIRE(level.get_entities().empty());

  REQUIRE(level.remove_entity(999) == nullptr);
}

TEST_CASE("Level entity radius query") {
  Level level(1, "L1");

  auto e1 = std::make_unique<Entity>();
  e1->set_id(1);
  level.spawn_entity(std::move(e1), Position{0, 0});
  auto e2 = std::make_unique<Entity>();
  e2->set_id(2);
  level.spawn_entity(std::move(e2), Position{3, 4});

  auto near = level.get_entities_radius(Position{0, 0}, 4);
  REQUIRE(near.size() == 1);

  auto all = level.get_entities_radius(Position{0, 0}, 5);
  REQUIRE(all.size() == 2);

  auto none = level.get_entities_radius(Position{0, 0}, -1);
  REQUIRE(none.empty());
}

TEST_CASE("Level move_entity updates position") {
  Level level(1, "L1");
  level.resize_field(3, 3);

  auto e1 = std::make_unique<Entity>();
  e1->set_id(5);
  level.spawn_entity(std::move(e1), Position{0, 0});
  REQUIRE(level.get_entities_radius(Position{0, 0}, 0).size() == 1);
  REQUIRE(level.move_entity(5, Position{2, 2}));
  REQUIRE(level.get_entities_radius(Position{0, 0}, 0).empty());
  REQUIRE(level.get_entities_radius(Position{2, 2}, 0).size() == 1);
  REQUIRE_FALSE(level.move_entity(5, Position{3, 3}));
  REQUIRE_FALSE(level.move_entity(999, Position{1, 1}));
}

TEST_CASE("Level add puts items into cell containers") {
  Level level(1, "L1");
  level.resize_field(2, 1);
  level.set_cell(Position{0, 0}, std::make_unique<Glass>());
  level.set_cell(Position{1, 0}, std::make_unique<Floor>());

  auto* cell = level.get_cell(Position{0, 0});
  auto* container = dynamic_cast<IItemContainer*>(cell);
  REQUIRE(container != nullptr);
  REQUIRE(container->size() == 0);

  std::unique_ptr<Item> item = std::make_unique<Medkit>(1, 1, 0, 1);
  auto returned = level.add(Position{0, 0}, std::move(item));
  REQUIRE(returned != nullptr);
  REQUIRE(container->size() == 0);

  REQUIRE(level.try_shoot(Position{0, 0}));
  returned = level.add(Position{0, 0}, std::move(returned));
  REQUIRE(returned == nullptr);
  auto* new_cell = level.get_cell(Position{0, 0});
  auto* new_container = dynamic_cast<IItemContainer*>(new_cell);
  REQUIRE(new_container != nullptr);
  REQUIRE(new_container->size() == 1);
}

}
