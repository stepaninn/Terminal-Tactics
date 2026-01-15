#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/service/TurnService.h"
#include "model/service/MovementService.h"
#include "model/service/CombatService.h"
#include "model/service/ItemService.h"
#include "model/service/InventoryService.h"
#include "model/service/LoadSaveService.h"
#include "model/service/World.h"
#include "model/service/VisionService.h"
#include "model/service/AIService.h"
#include "model/service/UseContext.h"
#include "model/repository/cells/Floor.h"
#include "model/repository/cells/Glass.h"
#include "model/repository/cells/ItemContainer.h"
#include "model/repository/cells/Stash.h"
#include "model/repository/cells/Wall.h"
#include "model/service/events/EventBus.h"
#include "model/service/events/Event.h"
#include "model/entity/EntityFactory.h"
#include "model/entity/components/CombatComponent.h"
#include "model/entity/components/AIComponent.h"
#include "model/entity/components/HealthComponent.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/components/MeleeComponent.h"
#include "model/entity/components/MoveComponent.h"
#include "model/entity/components/TimePointsComponent.h"
#include "model/entity/components/WeaponComponent.h"
#include "model/entity/components/VisionComponent.h"
#include "model/entity/entities/items/Item.h"
#include "model/entity/entities/items/Weapon.h"

#include <memory>
#include <vector>
#include <filesystem>

namespace game {

using entity::Entity;
using entity::factory::EntityFactory;
using entity::components::DefaultCombatComp;
using entity::components::DefaultHealthComp;
using entity::components::DefaultInventoryComp;
using entity::components::DefaultMoveComp;
using entity::components::DefaultTimePointsComp;
using entity::components::DefaultWeaponComp;
using entity::components::DefaultVisionComp;
using entity::components::CombatComponent;
using entity::components::HealthComponent;
using entity::components::InventoryComponent;
using entity::components::MoveComponent;
using entity::components::TimePointsComponent;
using entity::components::WeaponComponent;
using entity::components::VisionComponent;
using entity::items::AmmoBag;
using entity::items::Medkit;
using entity::items::Weapon;
using repo::Level;
using repo::cells::Floor;
using repo::cells::Glass;
using repo::cells::Stash;
using repo::cells::Wall;
using service::MovementService;
using service::InventoryService;
using service::World;
using service::VisionService;
using service::AIService;
using service::CombatService;
using service::ItemService;
using service::LoadSaveService;
using service::events::EventBus;
using service::events::ItemUsedEvent;
using service::events::DamageEvent;

static std::unique_ptr<Level> make_level(size_t w, size_t h) {
  auto level = std::make_unique<Level>(1, "L");
  level->resize_field(w, h);
  for (size_t x = 0; x < w; ++x) {
    for (size_t y = 0; y < h; ++y) {
      level->set_cell(Position{static_cast<int>(x), static_cast<int>(y)}, std::make_unique<Floor>());
    }
  }
  return level;
}

TEST_CASE("service TurnService cycles teams and refreshes action points") {
  auto level = std::make_shared<Level>(1, "L1");

  auto e0 = std::make_unique<Entity>();
  e0->set_id(10);
  e0->set_team_id(0);
  e0->add_component<TimePointsComponent, DefaultTimePointsComp>(0, 3);
  level->spawn_entity(std::move(e0), Position{0, 0});

  auto e1 = std::make_unique<Entity>();
  e1->set_id(20);
  e1->set_team_id(1);
  e1->add_component<TimePointsComponent, DefaultTimePointsComp>(0, 4);
  level->spawn_entity(std::move(e1), Position{0, 0});

  service::TurnService turn({0, 1});
  REQUIRE(turn.active_team() == 0);

  REQUIRE(turn.next_team(*level));
  REQUIRE(turn.active_team() == 1);

  auto* t0 = level->get_entity(10)->get_component<TimePointsComponent>();
  auto* t1 = level->get_entity(20)->get_component<TimePointsComponent>();
  REQUIRE(t0->get_current_points() == 0);
  REQUIRE(t1->get_current_points() == 4);
}

TEST_CASE("service TurnService selection and end turn") {
  Level level(1, "L1");

  auto e0 = std::make_unique<Entity>();
  e0->set_id(1);
  e0->set_team_id(0);
  level.spawn_entity(std::move(e0), Position{0, 0});

  auto e1 = std::make_unique<Entity>();
  e1->set_id(2);
  e1->set_team_id(1);
  level.spawn_entity(std::move(e1), Position{0, 0});

  service::TurnService turn({0, 1});
  REQUIRE(turn.select_entity(level, 1));
  REQUIRE(turn.active_entity() == 1);
  REQUIRE(turn.end_entity_turn());
  REQUIRE(turn.active_entity() == service::TurnService::kNoEntity);

  turn.set_active_team(0);
  REQUIRE_FALSE(turn.select_entity(level, 2));
}

TEST_CASE("service TurnService handles empty team list") {
  Level level(1, "L1");
  service::TurnService turn(std::vector<game::TeamId>{});
  REQUIRE_FALSE(turn.next_team(level));
}

TEST_CASE("service MovementService spends points and updates position") {
  auto level = std::make_shared<Level>(1, "L1");
  level->resize_field(2, 2);
  for (int r = 0; r < 2; ++r) {
    for (int c = 0; c < 2; ++c) {
      level->set_cell(Position{r, c}, std::make_unique<Floor>());
    }
  }

  auto mover = std::make_unique<Entity>();
  mover->set_id(7);
  mover->set_team_id(0);
  mover->add_component<MoveComponent, DefaultMoveComp>(1);
  mover->add_component<TimePointsComponent, DefaultTimePointsComp>(3, 3);
  level->spawn_entity(std::move(mover), Position{0, 0});

  service::MovementService movement;
  REQUIRE(movement.move(*level, 7, Position{1, 0}));

  auto* pos = level->get_entity_position(7);
  auto* tp = level->get_entity(7)->get_component<TimePointsComponent>();
  REQUIRE(pos != nullptr);
  REQUIRE(pos->x == 1);
  REQUIRE(pos->y == 0);
  REQUIRE(tp->get_current_points() == 2);
}

TEST_CASE("service MovementService finds path and respects blockers") {
  Level level(1, "L1");
  level.resize_field(3, 3);
  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c) {
      level.set_cell(Position{r, c}, std::make_unique<Floor>());
    }
  }

  auto e0 = std::make_unique<Entity>();
  e0->set_id(1);
  e0->add_component<MoveComponent, DefaultMoveComp>(1);
  e0->add_component<TimePointsComponent, DefaultTimePointsComp>(10, 10);
  level.spawn_entity(std::move(e0), Position{0, 0});

  MovementService movement;
  auto path = movement.find_path(level, 1, Position{2, 0});
  REQUIRE(path.size() == 2);

  Level blocked(1, "L1");
  blocked.resize_field(3, 1);
  for (int r = 0; r < 3; ++r) {
    blocked.set_cell(Position{r, 0}, std::make_unique<Floor>());
  }
  auto b0 = std::make_unique<Entity>();
  b0->set_id(10);
  b0->add_component<MoveComponent, DefaultMoveComp>(1);
  b0->add_component<TimePointsComponent, DefaultTimePointsComp>(10, 10);
  blocked.spawn_entity(std::move(b0), Position{0, 0});

  auto b1 = std::make_unique<Entity>();
  b1->set_id(11);
  blocked.spawn_entity(std::move(b1), Position{1, 0});
  REQUIRE(movement.find_path(blocked, 10, Position{2, 0}).empty());

  Level diag(1, "L1");
  diag.resize_field(2, 2);
  for (int r = 0; r < 2; ++r) {
    for (int c = 0; c < 2; ++c) {
      diag.set_cell(Position{r, c}, std::make_unique<Floor>());
    }
  }
  diag.set_cell(Position{0, 1}, std::make_unique<Wall>());
  diag.set_cell(Position{1, 0}, std::make_unique<Wall>());
  auto d0 = std::make_unique<Entity>();
  d0->set_id(20);
  d0->add_component<MoveComponent, DefaultMoveComp>(1);
  d0->add_component<TimePointsComponent, DefaultTimePointsComp>(10, 10);
  diag.spawn_entity(std::move(d0), Position{0, 0});
  REQUIRE(movement.find_path(diag, 20, Position{1, 1}).empty());
}

TEST_CASE("service MovementService path without target excludes final cell") {
  Level level(1, "L1");
  level.resize_field(3, 1);
  for (int r = 0; r < 3; ++r) {
    level.set_cell(Position{r, 0}, std::make_unique<Floor>());
  }

  auto e0 = std::make_unique<Entity>();
  e0->set_id(1);
  e0->add_component<MoveComponent, DefaultMoveComp>(1);
  e0->add_component<TimePointsComponent, DefaultTimePointsComp>(10, 10);
  level.spawn_entity(std::move(e0), Position{0, 0});

  MovementService movement;
  auto path = movement.find_path_without_target(level, 1, Position{2, 0});
  REQUIRE(path.size() == 1);
  REQUIRE(path[0].x == 1);
  REQUIRE(path[0].y == 0);
}

TEST_CASE("service CombatService can_shoot checks ammo and time") {
  auto attacker = std::make_unique<Entity>();
  attacker->add_component<CombatComponent, DefaultCombatComp>(1.0);
  attacker->add_component<TimePointsComponent, DefaultTimePointsComp>(1, 5);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 2, 1, AmmoType::PISTOL, 1, 5);
  attacker->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));

  REQUIRE_FALSE(service::CombatService::can_shoot(*attacker));
  attacker->get_component<TimePointsComponent>()->add_points(2);
  REQUIRE(service::CombatService::can_shoot(*attacker));
}

TEST_CASE("service CombatService blocks shooting without ammo") {
  auto level = std::make_shared<Level>(1, "L1");

  auto attacker = std::make_unique<Entity>();
  attacker->set_id(1);
  attacker->add_component<CombatComponent, DefaultCombatComp>(1.0);
  attacker->add_component<TimePointsComponent, DefaultTimePointsComp>(5, 5);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 2, 1, AmmoType::PISTOL, 0, 5);
  attacker->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(attacker), Position{0, 0});

  auto target = std::make_unique<Entity>();
  target->set_id(2);
  target->add_component<HealthComponent, DefaultHealthComp>(5, 5);
  level->spawn_entity(std::move(target), Position{1, 0});

  service::CombatService combat;
  auto* tp = level->get_entity(1)->get_component<TimePointsComponent>();
  auto* wc = level->get_entity(1)->get_component<WeaponComponent>();
  auto* hp = level->get_entity(2)->get_component<HealthComponent>();
  REQUIRE(combat.try_shoot(*level, 3, 4) == 0);
  REQUIRE(tp->get_current_points() == 5);
  REQUIRE(wc->get_weapon()->get_current_ammo() == 0);
  REQUIRE(hp->get_current_hp() == 5);
}

TEST_CASE("service CombatService blocks shooting without time points") {
  auto level = std::make_shared<Level>(1, "L1");

  auto attacker = std::make_unique<Entity>();
  attacker->set_id(3);
  attacker->add_component<CombatComponent, DefaultCombatComp>(1.0);
  attacker->add_component<TimePointsComponent, DefaultTimePointsComp>(0, 5);
  auto weapon = std::make_unique<Weapon>(3, 1, Damage{1, 2}, 5, 2, 1, AmmoType::PISTOL, 3, 5);
  attacker->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(attacker), Position{0, 0});

  auto target = std::make_unique<Entity>();
  target->set_id(4);
  target->add_component<HealthComponent, DefaultHealthComp>(5, 5);
  level->spawn_entity(std::move(target), Position{1, 0});

  service::CombatService combat;
  auto* tp = level->get_entity(3)->get_component<TimePointsComponent>();
  auto* wc = level->get_entity(3)->get_component<WeaponComponent>();
  auto* hp = level->get_entity(4)->get_component<HealthComponent>();
  REQUIRE(combat.try_shoot(*level, 1, 2) == 0);
  REQUIRE(tp->get_current_points() == 0);
  REQUIRE(wc->get_weapon()->get_current_ammo() == 3);
  REQUIRE(hp->get_current_hp() == 5);
}

TEST_CASE("service CombatService shooting damages target and emits events") {
  auto level = std::make_shared<Level>(1, "L1");
  level->resize_field(2, 1);
  level->set_cell(Position{0, 0}, std::make_unique<Floor>());
  level->set_cell(Position{1, 0}, std::make_unique<Floor>());

  auto attacker = std::make_unique<Entity>();
  attacker->set_id(1);
  attacker->add_component<CombatComponent, DefaultCombatComp>(1.0);
  attacker->add_component<TimePointsComponent, DefaultTimePointsComp>(5, 5);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{2, 2}, 5, 1, 1, AmmoType::PISTOL, 2, 2);
  attacker->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(attacker), Position{0, 0});

  auto target = std::make_unique<Entity>();
  target->set_id(2);
  target->add_component<HealthComponent, DefaultHealthComp>(2, 2);
  level->spawn_entity(std::move(target), Position{1, 0});

  auto bus = std::make_shared<EventBus>();
  int dmg_events = 0;
  int death_events = 0;
  bus->subscribe<DamageEvent>([&](const std::shared_ptr<DamageEvent>&) { ++dmg_events; });
  bus->subscribe<service::events::EntityDiedEvent>([&](const std::shared_ptr<service::events::EntityDiedEvent>&) {
    ++death_events;
  });

  service::CombatService combat(0, bus);
  for (int i = 0; i < 10; ++i) {
    if (level->get_entity(2)->get_component<HealthComponent>()->get_current_hp() == 0) break;
    REQUIRE(combat.try_shoot(*level, 1, 2));
  }
  bus->process();
  auto* hp = level->get_entity(2)->get_component<HealthComponent>();
  REQUIRE(hp->get_current_hp() == 0);
  REQUIRE(dmg_events >= 1);
  REQUIRE(death_events == 1);
}

TEST_CASE("service CombatService can shoot destructible cell by position") {
  auto level = std::make_shared<Level>(1, "L1");
  level->resize_field(3, 1);
  level->set_cell(Position{0, 0}, std::make_unique<Floor>());
  level->set_cell(Position{1, 0}, std::make_unique<Glass>());
  level->set_cell(Position{2, 0}, std::make_unique<Floor>());

  auto attacker = std::make_unique<Entity>();
  attacker->set_id(1);
  attacker->add_component<CombatComponent, DefaultCombatComp>(1.0);
  attacker->add_component<TimePointsComponent, DefaultTimePointsComp>(10, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 1}, 5, 1, 1, AmmoType::PISTOL, 10, 10);
  attacker->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(attacker), Position{0, 0});

  auto bus = std::make_shared<EventBus>();
  int broken_events = 0;
  bus->subscribe<service::events::WallBrokenEvent>(
      [&](const std::shared_ptr<service::events::WallBrokenEvent>&) { ++broken_events; });

  service::CombatService combat(1, bus);
  bool destroyed = false;
  for (int i = 0; i < 10; ++i) {
    (void)combat.try_shoot(*level, 1, Position{1, 0});
    destroyed = dynamic_cast<Floor*>(level->get_cell(Position{1, 0})) != nullptr;
    if (destroyed) break;
  }
  bus->process();

  REQUIRE(destroyed);
  REQUIRE(broken_events == 1);
  REQUIRE(dynamic_cast<Floor*>(level->get_cell(Position{1, 0})) != nullptr);
}

TEST_CASE("service CombatService cannot shoot through blocking cell by position") {
  auto level = std::make_shared<Level>(1, "L1");
  level->resize_field(3, 1);
  level->set_cell(Position{0, 0}, std::make_unique<Floor>());
  level->set_cell(Position{1, 0}, std::make_unique<Wall>());
  level->set_cell(Position{2, 0}, std::make_unique<Glass>());

  auto attacker = std::make_unique<Entity>();
  attacker->set_id(1);
  attacker->add_component<CombatComponent, DefaultCombatComp>(1.0);
  attacker->add_component<TimePointsComponent, DefaultTimePointsComp>(10, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 1}, 5, 1, 1, AmmoType::PISTOL, 10, 10);
  attacker->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(attacker), Position{0, 0});

  service::CombatService combat;
  REQUIRE_FALSE(combat.try_shoot(*level, 1, Position{2, 0}));
}

TEST_CASE("service CombatService performs melee attack in range") {
  auto level = std::make_shared<Level>(1, "L1");

  auto attacker = std::make_unique<Entity>();
  attacker->set_id(1);
  attacker->add_component<TimePointsComponent, DefaultTimePointsComp>(3, 3);
  attacker->add_component<entity::components::MeleeComponent, entity::components::DefaultMeleeComp>(2, 1);
  level->spawn_entity(std::move(attacker), Position{0, 0});

  auto target = std::make_unique<Entity>();
  target->set_id(2);
  target->add_component<HealthComponent, DefaultHealthComp>(2, 2);
  level->spawn_entity(std::move(target), Position{1, 0});

  service::CombatService combat;
  REQUIRE(combat.melee_attack(*level, 1, 2));
  auto* hp = level->get_entity(2)->get_component<HealthComponent>();
  REQUIRE(hp->get_current_hp() == 0);
}

TEST_CASE("service CombatService rejects melee out of range") {
  auto level = std::make_shared<Level>(1, "L1");

  auto attacker = std::make_unique<Entity>();
  attacker->set_id(1);
  attacker->add_component<TimePointsComponent, DefaultTimePointsComp>(3, 3);
  attacker->add_component<entity::components::MeleeComponent, entity::components::DefaultMeleeComp>(2, 1);
  level->spawn_entity(std::move(attacker), Position{0, 0});

  auto target = std::make_unique<Entity>();
  target->set_id(2);
  target->add_component<HealthComponent, DefaultHealthComp>(2, 2);
  level->spawn_entity(std::move(target), Position{3, 0});

  service::CombatService combat;
  REQUIRE_FALSE(combat.melee_attack(*level, 1, 2));
}

TEST_CASE("service CombatService reloads weapon and consumes time points") {
  auto level = std::make_shared<Level>(1, "L1");

  auto user = std::make_unique<Entity>();
  user->set_id(5);
  user->add_component<TimePointsComponent, DefaultTimePointsComp>(2, 2);
  user->add_component<InventoryComponent, DefaultInventoryComp>(2, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 2, 2, AmmoType::PISTOL, 0, 5);
  user->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(user), Position{0, 0});

  auto* inv = level->get_entity(5)->get_component<InventoryComponent>();
  inv->add(std::make_unique<AmmoBag>(200, 1, 3, 10, AmmoType::PISTOL));

  service::CombatService combat;
  REQUIRE(combat.reload_weapon(*level, 5));
  auto* tp = level->get_entity(5)->get_component<TimePointsComponent>();
  auto* wc = level->get_entity(5)->get_component<WeaponComponent>();
  REQUIRE(tp->get_current_points() == 0);
  REQUIRE(wc->get_weapon()->get_current_ammo() == 3);
  REQUIRE(inv->get_item(200) == nullptr);
}

TEST_CASE("service CombatService keeps ammo bag after partial reload") {
  auto level = std::make_shared<Level>(1, "L1");

  auto user = std::make_unique<Entity>();
  user->set_id(6);
  user->add_component<TimePointsComponent, DefaultTimePointsComp>(2, 2);
  user->add_component<InventoryComponent, DefaultInventoryComp>(2, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 2, 2, AmmoType::PISTOL, 3, 5);
  user->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(user), Position{0, 0});

  auto* inv = level->get_entity(6)->get_component<InventoryComponent>();
  inv->add(std::make_unique<AmmoBag>(201, 1, 10, 10, AmmoType::PISTOL));

  service::CombatService combat;
  REQUIRE(combat.reload_weapon(*level, 6));
  auto* tp = level->get_entity(6)->get_component<TimePointsComponent>();
  auto* wc = level->get_entity(6)->get_component<WeaponComponent>();
  auto* bag = dynamic_cast<AmmoBag*>(inv->get_item(201));
  REQUIRE(tp->get_current_points() == 0);
  REQUIRE(wc->get_weapon()->get_current_ammo() == 5);
  REQUIRE(bag != nullptr);
  REQUIRE(bag->get_current_ammo() == 8);
}

TEST_CASE("service CombatService reload fails without ammo bag") {
  auto level = std::make_shared<Level>(1, "L1");

  auto user = std::make_unique<Entity>();
  user->set_id(7);
  user->add_component<TimePointsComponent, DefaultTimePointsComp>(2, 2);
  user->add_component<InventoryComponent, DefaultInventoryComp>(2, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 2, 2, AmmoType::PISTOL, 0, 5);
  user->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(user), Position{0, 0});

  service::CombatService combat;
  REQUIRE_FALSE(combat.reload_weapon(*level, 7));
}

TEST_CASE("service ItemService uses medkit and ammo bag") {
  auto level = std::make_shared<Level>(1, "L1");
  auto user = std::make_unique<Entity>();
  user->set_id(8);
  user->add_component<HealthComponent, DefaultHealthComp>(5, 10);
  user->add_component<TimePointsComponent, DefaultTimePointsComp>(3, 3);
  user->add_component<InventoryComponent, DefaultInventoryComp>(3, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 3, 1, 1, AmmoType::PISTOL, 1, 5);
  user->add_component<WeaponComponent, DefaultWeaponComp>(std::move(weapon));
  level->spawn_entity(std::move(user), Position{0, 0});

  auto* inv = level->get_entity(8)->get_component<InventoryComponent>();
  inv->add(std::make_unique<Medkit>(100, 1, 3, 2));
  inv->add(std::make_unique<AmmoBag>(200, 1, 3, 10, AmmoType::PISTOL));

  service::ItemService items;
  REQUIRE(items.use_item(*level, 8, 8, 100));
  auto* hp = level->get_entity(8)->get_component<HealthComponent>();
  auto* tp = level->get_entity(8)->get_component<TimePointsComponent>();
  REQUIRE(hp->get_current_hp() == 8);
  REQUIRE(tp->get_current_points() == 1);
  REQUIRE(inv->get_item(100) == nullptr);

  REQUIRE(items.use_item(*level, 8, 8, 200));
  auto* wc = level->get_entity(8)->get_component<WeaponComponent>();
  REQUIRE(wc->get_weapon()->get_current_ammo() == 4);
  REQUIRE(inv->get_item(200) == nullptr);
}

TEST_CASE("service ItemService fails without time points or inventory") {
  auto level = std::make_shared<Level>(1, "L1");
  auto user = std::make_unique<Entity>();
  user->set_id(10);
  user->add_component<TimePointsComponent, DefaultTimePointsComp>(0, 5);
  user->add_component<InventoryComponent, DefaultInventoryComp>(2, 10);
  user->add_component<HealthComponent, DefaultHealthComp>(1, 5);
  level->spawn_entity(std::move(user), Position{0, 0});

  auto* inv = level->get_entity(10)->get_component<InventoryComponent>();
  inv->add(std::make_unique<Medkit>(400, 1, 3, 2));

  service::ItemService items;
  REQUIRE_FALSE(items.use_item(*level, 10, 10, 400));
  REQUIRE(inv->get_item(400) != nullptr);

  auto no_inv = std::make_unique<Entity>();
  no_inv->set_id(11);
  level->spawn_entity(std::move(no_inv), Position{0, 0});
  REQUIRE_FALSE(items.use_item(*level, 11, 11, 400));
}

TEST_CASE("service ItemService publishes ItemUsedEvent") {
  auto level = std::make_shared<Level>(1, "L1");
  auto user = std::make_unique<Entity>();
  user->set_id(12);
  user->add_component<TimePointsComponent, DefaultTimePointsComp>(5, 5);
  user->add_component<InventoryComponent, DefaultInventoryComp>(2, 10);
  user->add_component<HealthComponent, DefaultHealthComp>(1, 5);
  level->spawn_entity(std::move(user), Position{0, 0});

  auto* inv = level->get_entity(12)->get_component<InventoryComponent>();
  inv->add(std::make_unique<Medkit>(500, 1, 3, 2));

  auto bus = std::make_shared<EventBus>();
  int events = 0;
  bus->subscribe<ItemUsedEvent>([&](const std::shared_ptr<ItemUsedEvent>&) { ++events; });

  service::ItemService items(bus);
  REQUIRE(items.use_item(*level, 12, 12, 500));
  bus->process();
  REQUIRE(events == 1);
}

TEST_CASE("Medkit use heals and consumes time points") {
  DefaultHealthComp hp(1, 5);
  DefaultTimePointsComp tp(5, 5);
  DefaultInventoryComp inv(2, 10);

  inv.add(std::make_unique<Medkit>(800, 1, 3, 2));
  auto* item = inv.get_item(800);
  REQUIRE(item != nullptr);

  service::UseContext ctx{1, 1, &hp, &tp, nullptr, &inv};
  REQUIRE(item->use(ctx));
  REQUIRE(hp.get_current_hp() == 4);
  REQUIRE(tp.get_current_points() == 3);
  REQUIRE(inv.get_item(800) == nullptr);
}

TEST_CASE("Medkit use fails without time points and keeps item") {
  DefaultHealthComp hp(1, 5);
  DefaultTimePointsComp tp(0, 5);
  DefaultInventoryComp inv(2, 10);

  inv.add(std::make_unique<Medkit>(801, 1, 3, 2));
  auto* item = inv.get_item(801);
  REQUIRE(item != nullptr);

  service::UseContext ctx{1, 1, &hp, &tp, nullptr, &inv};
  REQUIRE_FALSE(item->use(ctx));
  REQUIRE(inv.get_item(801) != nullptr);
}

TEST_CASE("AmmoBag use reloads weapon and removes empty bag") {
  DefaultTimePointsComp tp(5, 5);
  DefaultInventoryComp inv(2, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 1, 2, AmmoType::PISTOL, 1, 5);
  DefaultWeaponComp wp(std::move(weapon));

  inv.add(std::make_unique<AmmoBag>(900, 1, 3, 5, AmmoType::PISTOL));
  auto* item = inv.get_item(900);
  REQUIRE(item != nullptr);

  service::UseContext ctx{1, 1, nullptr, &tp, &wp, &inv};
  REQUIRE(item->use(ctx));
  REQUIRE(wp.get_weapon()->get_current_ammo() == 4);
  REQUIRE(tp.get_current_points() == 3);
  REQUIRE(inv.get_item(900) == nullptr);
}

TEST_CASE("AmmoBag use keeps bag when ammo remains") {
  DefaultTimePointsComp tp(5, 5);
  DefaultInventoryComp inv(2, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 1, 1, AmmoType::PISTOL, 3, 5);
  DefaultWeaponComp wp(std::move(weapon));

  inv.add(std::make_unique<AmmoBag>(901, 1, 5, 5, AmmoType::PISTOL));
  auto* item = inv.get_item(901);
  REQUIRE(item != nullptr);

  service::UseContext ctx{1, 1, nullptr, &tp, &wp, &inv};
  REQUIRE(item->use(ctx));
  REQUIRE(wp.get_weapon()->get_current_ammo() == 5);
  REQUIRE(inv.get_item(901) != nullptr);
}

TEST_CASE("AmmoBag use fails when user is not target or ammo type mismatches") {
  DefaultTimePointsComp tp(5, 5);
  DefaultInventoryComp inv(2, 10);
  auto weapon = std::make_unique<Weapon>(1, 1, Damage{1, 2}, 5, 1, 1, AmmoType::PISTOL, 1, 5);
  DefaultWeaponComp wp(std::move(weapon));

  inv.add(std::make_unique<AmmoBag>(902, 1, 3, 5, AmmoType::RIFLE));
  auto* item = inv.get_item(902);
  REQUIRE(item != nullptr);

  service::UseContext wrong_user{1, 2, nullptr, &tp, &wp, &inv};
  REQUIRE_FALSE(item->use(wrong_user));

  service::UseContext wrong_ammo{1, 1, nullptr, &tp, &wp, &inv};
  REQUIRE_FALSE(item->use(wrong_ammo));
  REQUIRE(inv.get_item(902) != nullptr);
}

TEST_CASE("service InventoryService picks and drops items") {
  auto level = std::make_shared<Level>(1, "L1");
  level->resize_field(2, 1);
  level->set_cell(Position{0, 0}, std::make_unique<Floor>());

  auto user = std::make_unique<Entity>();
  user->set_id(9);
  user->add_component<InventoryComponent, DefaultInventoryComp>(2, 10);
  level->spawn_entity(std::move(user), Position{0, 0});

  level->add(Position{0, 0}, std::make_unique<Medkit>(300, 1, 2, 1));
  InventoryService inv_service;

  REQUIRE(inv_service.pick_item(*level, 9, 300));
  auto* inv = level->get_entity(9)->get_component<InventoryComponent>();
  REQUIRE(inv->get_item(300) != nullptr);

  REQUIRE(inv_service.drop_item(*level, 9, 300));
  REQUIRE(inv->get_item(300) == nullptr);
}

TEST_CASE("service InventoryService rejects invalid pick/drop") {
  auto level = std::make_shared<Level>(1, "L1");
  level->resize_field(1, 1);
  level->set_cell(Position{0, 0}, std::make_unique<Floor>());

  InventoryService inv_service;
  REQUIRE_FALSE(inv_service.pick_item(*level, 1, 1));
  REQUIRE_FALSE(inv_service.drop_item(*level, 1, 1));

  auto no_inv = std::make_unique<Entity>();
  no_inv->set_id(2);
  level->spawn_entity(std::move(no_inv), Position{0, 0});
  REQUIRE_FALSE(inv_service.pick_item(*level, 2, 1));
  REQUIRE_FALSE(inv_service.drop_item(*level, 2, 1));

  auto user = std::make_unique<Entity>();
  user->set_id(3);
  user->add_component<InventoryComponent, DefaultInventoryComp>(0, 0);
  level->spawn_entity(std::move(user), Position{0, 0});
  level->add(Position{0, 0}, std::make_unique<Medkit>(700, 5, 2, 1));
  REQUIRE_FALSE(inv_service.pick_item(*level, 3, 700));

  auto* inv = level->get_entity(3)->get_component<InventoryComponent>();
  inv->add(std::make_unique<Medkit>(701, 1, 2, 1));
  REQUIRE_FALSE(inv_service.pick_item(*level, 3, 701));

  REQUIRE_FALSE(inv_service.drop_item(*level, 3, 999));

  Level no_cell(1, "L1");
  no_cell.resize_field(1, 1);
  auto e = std::make_unique<Entity>();
  e->set_id(4);
  e->add_component<InventoryComponent, DefaultInventoryComp>(1, 10);
  no_cell.spawn_entity(std::move(e), Position{0, 0});
  auto* inv2 = no_cell.get_entity(4)->get_component<InventoryComponent>();
  inv2->add(std::make_unique<Medkit>(702, 1, 2, 1));
  REQUIRE_FALSE(inv_service.drop_item(no_cell, 4, 702));
}
TEST_CASE("service InventoryService rejects drop on blocked cell") {
  auto level = std::make_shared<Level>(1, "L1");
  level->resize_field(1, 1);
  level->set_cell(Position{0, 0}, std::make_unique<Wall>());

  auto user = std::make_unique<Entity>();
  user->set_id(20);
  user->add_component<InventoryComponent, DefaultInventoryComp>(2, 10);
  level->spawn_entity(std::move(user), Position{0, 0});

  auto* inv = level->get_entity(20)->get_component<InventoryComponent>();
  inv->add(std::make_unique<Medkit>(600, 1, 2, 1));

  InventoryService inv_service;
  REQUIRE_FALSE(inv_service.drop_item(*level, 20, 600));
  REQUIRE(inv->get_item(600) != nullptr);
}

TEST_CASE("VisibilityMap tracks visible and explored states") {
  service::VisibilityMap map(2, 2);
  REQUIRE_FALSE(map.is_visible(0, 0));
  REQUIRE_FALSE(map.is_explored(0, 0));

  map.set_visible(0, 0, true);
  REQUIRE(map.is_visible(0, 0));
  REQUIRE(map.is_explored(0, 0));

  map.clear_visible();
  REQUIRE_FALSE(map.is_visible(0, 0));
  REQUIRE(map.is_explored(0, 0));

  map.set_explored(1, 1, false);
  REQUIRE_FALSE(map.is_explored(1, 1));

  map.clear_all();
  REQUIRE_FALSE(map.is_visible(0, 0));
  REQUIRE_FALSE(map.is_explored(0, 0));
}

TEST_CASE("World tracks teams and visibility maps") {
  auto level = make_level(2, 2);
  World world(std::move(level));

  auto e1 = std::make_unique<Entity>();
  e1->set_id(1);
  e1->set_team_id(0);
  world.spawn_entity(std::move(e1), Position{0, 0});

  auto e2 = std::make_unique<Entity>();
  e2->set_id(2);
  e2->set_team_id(1);
  world.spawn_entity(std::move(e2), Position{1, 1});

  REQUIRE(world.get_team_entities(0).size() == 1);
  REQUIRE(world.get_team_entities(1).size() == 1);

  REQUIRE(world.set_entity_team(2, 0));
  REQUIRE(world.get_team_entities(0).size() == 2);
  REQUIRE(world.get_team_entities(1).empty());

  auto* v1 = world.unit_visibility(1);
  REQUIRE(v1 != nullptr);
  REQUIRE(v1->rows() == 2);
  REQUIRE(v1->cols() == 2);

  auto* tv = world.team_visibility(0);
  REQUIRE(tv != nullptr);
  auto* te = world.team_exploration(0);
  REQUIRE(te != nullptr);

  world.resize(3, 4);
  REQUIRE(v1->rows() == 3);
  REQUIRE(v1->cols() == 4);
  REQUIRE(tv->rows() == 3);
  REQUIRE(tv->cols() == 4);
  REQUIRE(te->rows() == 3);
  REQUIRE(te->cols() == 4);
}

TEST_CASE("World view filters components and removal updates teams") {
  auto level = make_level(2, 2);
  World world(std::move(level));

  auto e1 = std::make_unique<Entity>();
  e1->set_id(1);
  e1->set_team_id(0);
  e1->add_component<HealthComponent, DefaultHealthComp>(3, 3);
  world.spawn_entity(std::move(e1), Position{0, 0});

  auto e2 = std::make_unique<Entity>();
  e2->set_id(2);
  e2->set_team_id(0);
  world.spawn_entity(std::move(e2), Position{1, 1});

  auto view = world.view<HealthComponent>();
  REQUIRE(view.size() == 1);
  REQUIRE(view[0]->get_id() == 1);

  REQUIRE(world.remove_entity(1) != nullptr);
  REQUIRE(world.get_team_entities(0).size() == 1);
}

TEST_CASE("VisionService checks line of fire and visibility") {
  auto level = make_level(5, 5);
  level->set_cell(Position{2, 2}, std::make_unique<Wall>());

  World world(std::move(level));
  auto observer = std::make_unique<Entity>();
  observer->set_id(1);
  observer->set_team_id(0);
  observer->add_component<VisionComponent, DefaultVisionComp>(3);
  world.spawn_entity(std::move(observer), Position{0, 0});

  VisionService vision;
  vision.update_unit_fov(world, 1);
  auto* ufov = world.unit_visibility(1);
  REQUIRE(ufov != nullptr);
  REQUIRE(ufov->is_visible(Position{0, 0}));

  auto* lvl = world.get_level();
  REQUIRE(lvl != nullptr);
  REQUIRE_FALSE(vision.has_line_of_fire(*lvl, Position{0, 0}, Position{4, 4}));
  REQUIRE(vision.has_line_of_fire(*lvl, Position{0, 0}, Position{0, 4}));
}

TEST_CASE("VisionService lists visible entities and respects sees-items-only") {
  auto level = make_level(5, 5);
  World world(std::move(level));

  auto observer = std::make_unique<Entity>();
  observer->set_id(1);
  observer->set_team_id(0);
  observer->add_component<VisionComponent, DefaultVisionComp>(3, true);
  world.spawn_entity(std::move(observer), Position{0, 0});

  auto target = std::make_unique<Entity>();
  target->set_id(2);
  target->set_team_id(1);
  target->add_component<HealthComponent, DefaultHealthComp>(5, 5);
  world.spawn_entity(std::move(target), Position{2, 0});

  VisionService vision;
  auto seen = vision.visible_entities(world, 1);
  REQUIRE(seen.empty());

  world.get_entity(1)->remove_component<VisionComponent>();
  world.get_entity(1)->add_component<VisionComponent, DefaultVisionComp>(3, false);
  seen = vision.visible_entities(world, 1);
  REQUIRE(seen.size() == 1);
  REQUIRE(seen[0] == 2);
}

TEST_CASE("VisionService rebuilds team visibility and exploration") {
  auto level = make_level(4, 4);
  World world(std::move(level));

  auto e1 = std::make_unique<Entity>();
  e1->set_id(1);
  e1->set_team_id(0);
  e1->add_component<VisionComponent, DefaultVisionComp>(2);
  world.spawn_entity(std::move(e1), Position{1, 1});

  auto e2 = std::make_unique<Entity>();
  e2->set_id(2);
  e2->set_team_id(0);
  e2->add_component<VisionComponent, DefaultVisionComp>(2);
  world.spawn_entity(std::move(e2), Position{2, 2});

  VisionService vision;
  vision.update_unit_fov(world, 1);
  vision.update_unit_fov(world, 2);
  vision.rebuild_team_visible(world, 0);

  auto* team_vis = world.team_visibility(0);
  auto* explored = world.team_exploration(0);
  REQUIRE(team_vis != nullptr);
  REQUIRE(explored != nullptr);
  REQUIRE(team_vis->is_visible(Position{1, 1}));
  REQUIRE(explored->is_explored(Position{1, 1}));
}

TEST_CASE("VisionService rebuilds all FOV and handles edge cases") {
  auto level = make_level(3, 3);
  level->set_cell(Position{1, 1}, std::make_unique<Wall>());
  World world(std::move(level));

  auto e1 = std::make_unique<Entity>();
  e1->set_id(1);
  e1->set_team_id(0);
  e1->add_component<VisionComponent, DefaultVisionComp>(0);
  world.spawn_entity(std::move(e1), Position{0, 0});

  auto e2 = std::make_unique<Entity>();
  e2->set_id(2);
  e2->set_team_id(1);
  e2->add_component<VisionComponent, DefaultVisionComp>(2);
  world.spawn_entity(std::move(e2), Position{2, 2});

  VisionService vision;
  vision.rebuild_all_fov(world);

  auto* ufov = world.unit_visibility(1);
  REQUIRE(ufov != nullptr);
  REQUIRE(ufov->is_visible(Position{0, 0}));
  REQUIRE_FALSE(vision.has_line_of_fire(*world.get_level(), Position{0, 0}, Position{5, 5}));
  REQUIRE(vision.has_line_of_fire(*world.get_level(), Position{0, 0}, Position{0, 0}));
}

TEST_CASE("AIService chooses melee and shooting actions") {
  auto level = make_level(5, 5);
  World world(std::move(level));

  auto wild_weapon = std::make_unique<Weapon>(10, 1, Damage{1, 2}, 3, 1, 1, AmmoType::PISTOL, 2, 4);
  auto wild = EntityFactory::create_wild(1, "Wild", std::move(wild_weapon));
  wild->set_team_id(0);
  world.spawn_entity(std::move(wild), Position{1, 1});

  auto target = std::make_unique<Entity>();
  target->set_id(2);
  target->set_team_id(1);
  target->add_component<HealthComponent, DefaultHealthComp>(10, 10);
  world.spawn_entity(std::move(target), Position{1, 2});

  MovementService move;
  CombatService combat;
  ItemService items;
  VisionService vision;
  AIService ai(move, combat, items, vision);

  ai.act_entity(world, 1);
  auto* hp = world.get_entity(2)->get_component<HealthComponent>();
  REQUIRE(hp != nullptr);
  REQUIRE(hp->get_current_hp() == 7);

  auto sniper_weapon = std::make_unique<Weapon>(20, 1, Damage{1, 2}, 5, 1, 1, AmmoType::RIFLE, 3, 5);
  auto sniper = EntityFactory::create_intelligent(3, "Sniper", std::move(sniper_weapon));
  sniper->set_team_id(0);
  world.spawn_entity(std::move(sniper), Position{0, 0});

  auto target2 = std::make_unique<Entity>();
  target2->set_id(4);
  target2->set_team_id(1);
  target2->add_component<HealthComponent, DefaultHealthComp>(10, 10);
  world.spawn_entity(std::move(target2), Position{0, 3});

  ai.act_entity(world, 3);
  auto* wp = world.get_entity(3)->get_component<entity::components::WeaponComponent>();
  auto* tp = world.get_entity(3)->get_component<entity::components::TimePointsComponent>();
  REQUIRE(wp->get_weapon()->get_current_ammo() == 2);
  REQUIRE(tp->get_current_points() == 7);
}

TEST_CASE("AIService picks up item from current cell") {
  auto level = make_level(3, 1);
  level->add(Position{0, 0}, std::make_unique<Medkit>(900, 1, 2, 1));

  World world(std::move(level));
  auto forager = EntityFactory::create_forager(1, "Forager", nullptr);
  world.spawn_entity(std::move(forager), Position{0, 0});

  MovementService move;
  CombatService combat;
  ItemService items;
  VisionService vision;
  AIService ai(move, combat, items, vision);

  ai.act_entity(world, 1);
  auto* inv = world.get_entity(1)->get_component<InventoryComponent>();
  REQUIRE(inv->size() == 1);
  auto* cell = world.get_level()->get_cell(Position{0, 0});
  auto* cont = dynamic_cast<repo::cells::IItemContainer*>(cell);
  REQUIRE(cont != nullptr);
  REQUIRE(cont->size() == 0);
}

TEST_CASE("AIService deposits items in stash") {
  auto level = make_level(3, 1);
  level->set_cell(Position{0, 0}, std::make_unique<Stash>());

  World world(std::move(level));
  auto forager = EntityFactory::create_forager(1, "Forager", nullptr);
  auto* inv = &forager->add_component<InventoryComponent, DefaultInventoryComp>(2, 10);
  inv->add(std::make_unique<Medkit>(901, 1, 2, 1));
  world.spawn_entity(std::move(forager), Position{0, 0});

  MovementService move;
  CombatService combat;
  ItemService items;
  VisionService vision;
  AIService ai(move, combat, items, vision);

  ai.act_entity(world, 1);
  auto* inv_after = world.get_entity(1)->get_component<InventoryComponent>();
  REQUIRE(inv_after->size() == 0);
  auto* stash = dynamic_cast<Stash*>(world.get_level()->get_cell(Position{0, 0}));
  REQUIRE(stash != nullptr);
  REQUIRE(stash->size() == 1);
}

TEST_CASE("AIService moves toward item and stash") {
  auto level = make_level(5, 1);
  level->set_cell(Position{4, 0}, std::make_unique<Stash>());
  level->add(Position{3, 0}, std::make_unique<Medkit>(902, 1, 2, 1));

  World world(std::move(level));
  auto forager = EntityFactory::create_forager(1, "Forager", nullptr);
  world.spawn_entity(std::move(forager), Position{0, 0});

  MovementService move;
  CombatService combat;
  ItemService items;
  VisionService vision;
  AIService ai(move, combat, items, vision);

  ai.act_entity(world, 1);
  auto* pos = world.get_level()->get_entity_position(1);
  REQUIRE(pos != nullptr);
  int x0 = pos->x;
  REQUIRE(x0 != 0);

  auto* inv = world.get_entity(1)->get_component<InventoryComponent>();
  inv->add(std::make_unique<Medkit>(903, 1, 2, 1));
  ai.act_entity(world, 1);
  auto* pos2 = world.get_level()->get_entity_position(1);
  REQUIRE(pos2 != nullptr);
  REQUIRE(pos2->x > x0);
}

TEST_CASE("AIService moves to stash when inventory is full") {
  auto level = make_level(4, 1);
  level->set_cell(Position{3, 0}, std::make_unique<Stash>());

  World world(std::move(level));
  auto forager = EntityFactory::create_forager(1, "Forager", nullptr);
  forager->replace_component<InventoryComponent, DefaultInventoryComp>(1, 10);
  auto* inv = forager->get_component<InventoryComponent>();
  inv->add(std::make_unique<Medkit>(904, 1, 2, 1));
  world.spawn_entity(std::move(forager), Position{0, 0});

  MovementService move;
  CombatService combat;
  ItemService items;
  VisionService vision;
  AIService ai(move, combat, items, vision);

  ai.act_entity(world, 1);
  auto* pos = world.get_level()->get_entity_position(1);
  REQUIRE(pos != nullptr);
  REQUIRE(pos->x == 1);
}

TEST_CASE("AIService moves toward enemy and wanders when idle") {
  auto level = make_level(5, 1);
  World world(std::move(level));

  auto wild = EntityFactory::create_wild(1, "Wild", nullptr);
  world.spawn_entity(std::move(wild), Position{0, 0});

  auto target = std::make_unique<Entity>();
  target->set_id(2);
  target->set_team_id(1);
  world.spawn_entity(std::move(target), Position{4, 0});

  MovementService move;
  CombatService combat;
  ItemService items;
  VisionService vision;
  AIService ai(move, combat, items, vision);

  ai.act_entity(world, 1);
  auto* pos = world.get_level()->get_entity_position(1);
  REQUIRE(pos != nullptr);
  REQUIRE(pos->x == 1);

  auto idle = EntityFactory::create_wild(3, "Idle", nullptr);
  world.spawn_entity(std::move(idle), Position{2, 0});
  ai.act_entity(world, 3);
  auto* idle_pos = world.get_level()->get_entity_position(3);
  REQUIRE(idle_pos != nullptr);
  REQUIRE(idle_pos->x != 2);
}

TEST_CASE("EventBus delivers queued events") {
  EventBus bus;
  int seen = 0;
  bus.subscribe<DamageEvent>([&](const std::shared_ptr<DamageEvent>& ev) {
    if (ev->amount == 3) ++seen;
  });

  auto ev = std::make_shared<DamageEvent>();
  ev->attacker_id = 1;
  ev->target_id = 2;
  ev->amount = 3;
  bus.publish(ev);
  bus.process();

  REQUIRE(seen == 1);
}

TEST_CASE("LoadSaveService saves and loads world state") {
  auto level = std::make_unique<Level>(1, "SaveTest");
  level->resize_field(2, 2);
  level->set_cell(Position{0, 0}, std::make_unique<Floor>());

  auto glass = std::make_unique<Glass>();
  glass->break_glass();
  level->set_cell(Position{0, 1}, std::move(glass));

  level->set_cell(Position{1, 0}, std::make_unique<Stash>());
  level->set_cell(Position{1, 1}, std::make_unique<Wall>());

  level->add(Position{0, 0}, std::make_unique<Medkit>(1000, 1, 2, 1));
  level->add(Position{0, 1}, std::make_unique<AmmoBag>(1001, 1, 2, 4, AmmoType::PISTOL));
  level->add(Position{1, 0}, std::make_unique<Weapon>(1002, 2, Damage{1, 3}, 4, 1, 2,
                                                      AmmoType::RIFLE, 3, 5));

  auto ent = std::make_unique<Entity>(10, "Hero", 0);
  ent->add_component<HealthComponent, DefaultHealthComp>(5, 10);
  auto& inv = ent->add_component<InventoryComponent, DefaultInventoryComp>(4, 50);
  inv.add(std::make_unique<Medkit>(1100, 1, 3, 2));
  inv.add(std::make_unique<AmmoBag>(1101, 1, 3, 6, AmmoType::PISTOL));
  ent->add_component<MoveComponent, DefaultMoveComp>(1);
  ent->add_component<TimePointsComponent, DefaultTimePointsComp>(3, 5);
  ent->add_component<CombatComponent, DefaultCombatComp>(0.5);
  ent->add_component<entity::components::MeleeComponent, entity::components::DefaultMeleeComp>(2, 1);
  ent->add_component<entity::components::AIComponent, entity::components::DefaultAIComp>(
      entity::components::AIBehavior::WILD);
  ent->add_component<WeaponComponent, DefaultWeaponComp>(std::make_unique<Weapon>(
      1200, 2, Damage{2, 4}, 5, 1, 2, AmmoType::PISTOL, 1, 6));
  ent->add_component<VisionComponent, DefaultVisionComp>(4, false);
  level->spawn_entity(std::move(ent), Position{0, 0});

  World world(std::move(level));
  LoadSaveService ls;
  const std::string path = "test_save.txt";
  REQUIRE(ls.save(world, path));

  auto loaded = ls.load(path);
  REQUIRE(loaded != nullptr);
  auto* loaded_level = loaded->get_level();
  REQUIRE(loaded_level != nullptr);
  REQUIRE(loaded_level->get_name() == "SaveTest");
  REQUIRE(loaded_level->get_width() == 2);
  REQUIRE(loaded_level->get_height() == 2);

  auto* loaded_glass = dynamic_cast<Glass*>(loaded_level->get_cell(Position{0, 1}));
  REQUIRE(loaded_glass != nullptr);
  REQUIRE(loaded_glass->is_broken());
  auto* stash = dynamic_cast<Stash*>(loaded_level->get_cell(Position{1, 0}));
  REQUIRE(stash != nullptr);
  REQUIRE(stash->size() == 1);

  auto* loaded_ent = loaded_level->get_entity(10);
  REQUIRE(loaded_ent != nullptr);
  REQUIRE(loaded_ent->get_name() == "Hero");
  REQUIRE(loaded_ent->get_component<HealthComponent>() != nullptr);
  REQUIRE(loaded_ent->get_component<VisionComponent>() != nullptr);
  auto* loaded_inv = loaded_ent->get_component<InventoryComponent>();
  REQUIRE(loaded_inv != nullptr);
  REQUIRE(loaded_inv->size() == 2);
  auto* loaded_wp = loaded_ent->get_component<WeaponComponent>();
  REQUIRE(loaded_wp != nullptr);
  REQUIRE(loaded_wp->get_weapon()->get_ammo_type() == AmmoType::PISTOL);

  std::filesystem::remove(path);
}

}
