#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/repository/cells/Floor.h"
#include "model/repository/cells/Wall.h"
#include "model/repository/cells/Glass.h"
#include "model/repository/cells/Stash.h"
#include "model/repository/cells/Partition.h"
#include "model/repository/cells/ItemContainer.h"
#include "model/entity/entities/items/Item.h"
#include "types.h"

#include <algorithm>
#include <memory>
#include <vector>

namespace game {

using entity::items::AmmoBag;
using entity::items::Medkit;
using repo::cells::Floor;
using repo::cells::Glass;
using repo::cells::IItemContainer;
using repo::cells::ItemStorage;
using repo::cells::Partition;
using repo::cells::Stash;
using repo::cells::Wall;

TEST_CASE("ItemStorage basic operations") {
  ItemStorage storage;

  SECTION("add and get_item") {
    storage.add(std::make_unique<Medkit>(42, 1, 0, 5));
    REQUIRE(storage.size() == 1);
    REQUIRE(storage.get_item(42) != nullptr);
    REQUIRE(storage.get_item(99) == nullptr);
  }

  SECTION("remove_by_id returns element and shrinks size") {
    storage.add(std::make_unique<Medkit>(7, 1, 0, 5));
    auto removed = storage.remove_by_id(7);
    REQUIRE(removed != nullptr);
    REQUIRE(removed->get_id() == 7);
    REQUIRE(storage.size() == 0);
  }

  SECTION("remove_by_id misses safely") {
    REQUIRE(storage.remove_by_id(1) == nullptr);
    REQUIRE(storage.size() == 0);
  }

  SECTION("get_items returns stored pointers") {
    storage.add(std::make_unique<Medkit>(1, 1, 0, 1));
    storage.add(std::make_unique<AmmoBag>(2, 10, 0, 5, AmmoType::PISTOL));
    auto items = storage.get_items();
    REQUIRE(items.size() == 2);
    std::vector<id_t> ids;
    ids.reserve(items.size());
    for (auto* item : items) ids.push_back(item->get_id());
    std::ranges::sort(ids);
    REQUIRE(ids == std::vector<id_t>{1, 2});
  }
}

TEST_CASE("Floor cell stores items and reports properties") {
  Floor floor;

  REQUIRE(floor.is_walkable());
  REQUIRE_FALSE(floor.is_blocks_vision());
  REQUIRE(floor.can_shoot_through());
  REQUIRE(floor.can_place_items());
  REQUIRE(floor.view_name() == std::string_view("Floor"));
  REQUIRE(floor.size() == 0);

  auto medkit = std::make_unique<Medkit>(10, 1, 0, 3);
  auto ammo = std::make_unique<AmmoBag>(11, 5, 0, 2, AmmoType::PISTOL);

  REQUIRE(floor.add(std::move(medkit)) == nullptr);
  REQUIRE(floor.add(std::move(ammo)) == nullptr);
  REQUIRE(floor.size() == 2);
  REQUIRE(floor.get_item(10) != nullptr);
  REQUIRE(floor.get_item(11) != nullptr);

  auto removed = floor.remove_by_id(10);
  REQUIRE(removed != nullptr);
  REQUIRE(removed->get_id() == 10);
  REQUIRE(floor.size() == 1);
  REQUIRE(floor.get_item(10) == nullptr);
}

TEST_CASE("Wall cell blocks walking and vision") {
  Wall wall;
  REQUIRE_FALSE(wall.is_walkable());
  REQUIRE(wall.is_blocks_vision());
  REQUIRE_FALSE(wall.can_shoot_through());
  REQUIRE(wall.view_name() == std::string_view("Wall"));
  auto* container = dynamic_cast<IItemContainer*>(&wall);
  REQUIRE(container == nullptr);
}

TEST_CASE("Glass cell blocks walking but not vision") {
  Glass glass;
  REQUIRE_FALSE(glass.is_walkable());
  REQUIRE_FALSE(glass.is_blocks_vision());
  REQUIRE_FALSE(glass.can_shoot_through());
  REQUIRE_FALSE(glass.can_place_items());
  REQUIRE(glass.view_name() == std::string_view("Glass"));
  REQUIRE(glass.size() == 0);

  SECTION("breaking makes it walkable") {
    REQUIRE_FALSE(glass.is_broken());
    glass.break_glass();
    REQUIRE(glass.is_broken());
    REQUIRE(glass.is_walkable());
    REQUIRE(glass.can_shoot_through());
    REQUIRE(glass.can_place_items());
  }

  SECTION("apply_shot breaks only once") {
    REQUIRE(glass.apply_shot()); // первый выстрел разбивает стекло
    REQUIRE(glass.is_broken());
    REQUIRE(glass.is_walkable());
    REQUIRE(glass.can_shoot_through());
    REQUIRE_FALSE(glass.apply_shot()); // повторный не меняет состояние
  }
}

TEST_CASE("Stash behaves like a floor with different name") {
  Stash stash;
  REQUIRE(stash.is_walkable());
  REQUIRE_FALSE(stash.is_blocks_vision());
  REQUIRE(stash.can_shoot_through());
  REQUIRE(stash.can_place_items());
  REQUIRE(stash.view_name() == std::string_view("Stash"));

  std::unique_ptr<game::entity::items::Item> stash_item = std::make_unique<Medkit>(5, 1, 0, 4);
  REQUIRE(stash.add(std::move(stash_item)) == nullptr);
  REQUIRE(stash.size() == 1);
  REQUIRE(stash.get_item(5) != nullptr);
}

TEST_CASE("Partition blocks vision until broken") {
  Partition partition;
  REQUIRE_FALSE(partition.is_walkable());
  REQUIRE(partition.is_blocks_vision());
  REQUIRE_FALSE(partition.can_shoot_through());
  REQUIRE(partition.view_name() == std::string_view("Partition"));

  SECTION("apply_shot toggles state") {
    REQUIRE(partition.apply_shot()); // перегородка разбита выстрелом
    REQUIRE(partition.is_broken());
    REQUIRE(partition.is_walkable());
    REQUIRE_FALSE(partition.is_blocks_vision());
    REQUIRE(partition.can_shoot_through());
    REQUIRE_FALSE(partition.apply_shot());
  }
}
}
