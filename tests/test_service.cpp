#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/service/services.h"
#include "model/repository/cells/Floor.h"
#include "model/entity/components/MoveComponent.h"
#include "model/entity/components/TimePointsComponent.h"
#include "model/entity/components/PositionComponent.h"

#include <memory>

namespace game {

using entity::Entity;
using entity::components::DefaultMoveComp;
using entity::components::DefaultPositionComp;
using entity::components::DefaultTimePointsComp;
using entity::components::MoveComponent;
using entity::components::PositionComponent;
using entity::components::TimePointsComponent;
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
  mover->add_component<PositionComponent, DefaultPositionComp>(Position{0, 0});
  mover->add_component<MoveComponent, DefaultMoveComp>(1);
  mover->add_component<TimePointsComponent, DefaultTimePointsComp>(3, 3);
  level->spawn_entity(std::move(mover), Position{0, 0});

  service::MovementService movement;
  REQUIRE(movement.move(*level, 1, Position{1, 0}));

  auto* pos = level->get_entity(1)->get_component<PositionComponent>();
  auto* tp = level->get_entity(1)->get_component<TimePointsComponent>();
  REQUIRE(pos->get_position().x == 1);
  REQUIRE(pos->get_position().y == 0);
  REQUIRE(tp->get_current_points() == 2);
}

}
