#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/service/services.h"
#include "model/repository/cells/Floor.h"
#include "model/entity/components/CombatComponent.h"
#include "model/entity/components/HealthComponent.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/components/MoveComponent.h"
#include "model/entity/components/TimePointsComponent.h"
#include "model/entity/components/WeaponComponent.h"
#include "model/entity/entities/items/Item.h"
#include "model/entity/entities/items/Weapon.h"

#include <memory>

namespace game {

using entity::Entity;
using entity::components::DefaultCombatComp;
using entity::components::DefaultHealthComp;
using entity::components::DefaultInventoryComp;
using entity::components::DefaultMoveComp;
using entity::components::DefaultTimePointsComp;
using entity::components::DefaultWeaponComp;
using entity::components::CombatComponent;
using entity::components::HealthComponent;
using entity::components::InventoryComponent;
using entity::components::MoveComponent;
using entity::components::TimePointsComponent;
using entity::components::WeaponComponent;
using entity::items::AmmoBag;
using entity::items::Medkit;
using entity::items::Weapon;
using repo::Level;
using repo::cells::Floor;

TEST_CASE("service TurnService cycles teams and refreshes action points") {
  auto level = std::make_shared<Level>(1, "L1");

  auto e0 = std::make_unique<Entity>();
  e0->set_team_id(0);
  e0->add_component<TimePointsComponent, DefaultTimePointsComp>(0, 3);
  level->spawn_entity(std::move(e0), Position{0, 0});

  auto e1 = std::make_unique<Entity>();
  e1->set_team_id(1);
  e1->add_component<TimePointsComponent, DefaultTimePointsComp>(0, 4);
  level->spawn_entity(std::move(e1), Position{0, 0});

  service::TurnService turn({0, 1});
  REQUIRE(turn.active_team() == 0);

  REQUIRE(turn.next_team(*level));
  REQUIRE(turn.active_team() == 1);

  auto* t0 = level->get_entity(1)->get_component<TimePointsComponent>();
  auto* t1 = level->get_entity(2)->get_component<TimePointsComponent>();
  REQUIRE(t0->get_current_points() == 0);
  REQUIRE(t1->get_current_points() == 4);
}

TEST_CASE("service MovementService spends points and updates position") {
  auto level = std::make_shared<Level>(1, "L1");
  level->resize_field(2, 2);
  for (size_t r = 0; r < 2; ++r) {
    for (size_t c = 0; c < 2; ++c) {
      level->set_cell(Position{r, c}, std::make_unique<Floor>());
    }
  }

  auto mover = std::make_unique<Entity>();
  mover->set_team_id(0);
  mover->add_component<MoveComponent, DefaultMoveComp>(1);
  mover->add_component<TimePointsComponent, DefaultTimePointsComp>(3, 3);
  level->spawn_entity(std::move(mover), Position{0, 0});

  service::MovementService movement;
  REQUIRE(movement.move(*level, 1, Position{1, 0}));

  auto* pos = level->get_entity_position(1);
  auto* tp = level->get_entity(1)->get_component<TimePointsComponent>();
  REQUIRE(pos != nullptr);
  REQUIRE(pos->x == 1);
  REQUIRE(pos->y == 0);
  REQUIRE(tp->get_current_points() == 2);
}

TEST_CASE("service CombatService blocks shooting without ammo or time points") {
  auto level = std::make_shared<Level>(1, "L1");

  auto attacker = std::make_unique<Entity>();
  attacker->add_component<CombatComponent, DefaultCombatComp>(1.0);
  attacker->add_component<TimePointsComponent, DefaultTimePointsComp>(0, 5);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 2, 1, AmmoType::PISTOL, 0, 5);
  attacker->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(attacker), Position{0, 0});

  auto target = std::make_unique<Entity>();
  target->add_component<HealthComponent, DefaultHealthComp>(5, 5);
  level->spawn_entity(std::move(target), Position{1, 0});

  service::CombatService combat;
  auto* tp = level->get_entity(1)->get_component<TimePointsComponent>();
  auto* wc = level->get_entity(1)->get_component<WeaponComponent>();
  auto* hp = level->get_entity(2)->get_component<HealthComponent>();
  REQUIRE(combat.try_shoot(*level, 1, 2) == 0);
  REQUIRE(tp->get_current_points() == 0);
  REQUIRE(wc->get_weapon()->get_current_ammo() == 0);
  REQUIRE(hp->get_current_hp() == 5);
}

TEST_CASE("service CombatService reloads weapon and consumes time points") {
  auto level = std::make_shared<Level>(1, "L1");

  auto user = std::make_unique<Entity>();
  user->add_component<TimePointsComponent, DefaultTimePointsComp>(2, 2);
  user->add_component<InventoryComponent, DefaultInventoryComp>(2, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 2, 2, AmmoType::PISTOL, 0, 5);
  user->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(user), Position{0, 0});

  auto* inv = level->get_entity(1)->get_component<InventoryComponent>();
  inv->add(std::make_unique<AmmoBag>(200, 1, 3, 10, AmmoType::PISTOL));

  service::CombatService combat;
  REQUIRE(combat.reload_weapon(*level->get_entity(1), 200));
  auto* tp = level->get_entity(1)->get_component<TimePointsComponent>();
  auto* wc = level->get_entity(1)->get_component<WeaponComponent>();
  REQUIRE(tp->get_current_points() == 0);
  REQUIRE(wc->get_weapon()->get_current_ammo() == 3);
  REQUIRE(inv->get_item(200) == nullptr);
}

TEST_CASE("service ItemService uses medkit and ammo bag") {
  auto level = std::make_shared<Level>(1, "L1");
  auto user = std::make_unique<Entity>();
  user->add_component<HealthComponent, DefaultHealthComp>(5, 10);
  user->add_component<TimePointsComponent, DefaultTimePointsComp>(3, 3);
  user->add_component<InventoryComponent, DefaultInventoryComp>(3, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 3, 1, 1, AmmoType::PISTOL, 1, 5);
  user->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(user), Position{0, 0});

  auto* inv = level->get_entity(1)->get_component<InventoryComponent>();
  inv->add(std::make_unique<Medkit>(100, 1, 3, 2));
  inv->add(std::make_unique<AmmoBag>(200, 1, 3, 10, AmmoType::PISTOL));

  service::ItemService items;
  REQUIRE(items.use_item(*level, 1, 100));
  auto* hp = level->get_entity(1)->get_component<HealthComponent>();
  auto* tp = level->get_entity(1)->get_component<TimePointsComponent>();
  REQUIRE(hp->get_current_hp() == 8);
  REQUIRE(tp->get_current_points() == 1);
  REQUIRE(inv->get_item(100) == nullptr);

  REQUIRE(items.use_item(*level, 1, 200));
  auto* wc = level->get_entity(1)->get_component<WeaponComponent>();
  REQUIRE(wc->get_weapon()->get_current_ammo() == 4);
  REQUIRE(inv->get_item(200) == nullptr);
}

}
