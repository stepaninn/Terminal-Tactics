#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "model/entity/entities/Entity.h"
#include "model/entity/components/HealthComponent.h"
#include "model/entity/components/AIComponent.h"
#include "model/entity/components/PositionComponent.h"

namespace game {

TEST_CASE("Entity stores id and name") {
  Entity e(7, "unit");
  REQUIRE(e.get_id() == 7);
  REQUIRE(e.get_name() == std::string("unit"));
  e.set_id(9);
  e.set_name("npc");
  REQUIRE(e.get_id() == 9);
  REQUIRE(e.get_name() == std::string("npc"));
}

TEST_CASE("Entity add/get/remove component") {
  Entity e;
  auto& hc = e.add_component<HealthComponent, DefaultHealthComp>(3, 10);
  REQUIRE(hc.get_current_hp() == 3);
  REQUIRE(hc.get_max_hp() == 10);

  auto* hc_ptr = e.get_component<HealthComponent>();
  REQUIRE(hc_ptr != nullptr);
  REQUIRE(hc_ptr->get_current_hp() == 3);

  auto removed = e.remove_component<HealthComponent>();
  REQUIRE(removed != nullptr);
  REQUIRE(removed->get_current_hp() == 3);
  REQUIRE(e.get_component<HealthComponent>() == nullptr);
}

TEST_CASE("Entity supports multiple component types") {
  Entity e;
  e.add_component<AIComponent, DefaultAIComp>(AIState::AGGRESSIVE);
  e.add_component<PositionComponent, DefaultPositionComp>(Position{2, 4});

  auto* ai = e.get_component<AIComponent>();
  auto* pos = e.get_component<PositionComponent>();
  REQUIRE(ai != nullptr);
  REQUIRE(pos != nullptr);
  REQUIRE(ai->get_state() == AIState::AGGRESSIVE);
  REQUIRE(pos->get_position().x == 2);
  REQUIRE(pos->get_position().y == 4);
}

TEST_CASE("Entity replace component of same type") {
  Entity e;
  auto& h1 = e.add_component<HealthComponent, DefaultHealthComp>(1, 5);
  REQUIRE(h1.get_current_hp() == 1);
  auto& h2 = e.add_component<HealthComponent, DefaultHealthComp>(4, 7);
  REQUIRE(h2.get_current_hp() == 4);
  REQUIRE(h2.get_max_hp() == 7);

  auto* current = e.get_component<HealthComponent>();
  REQUIRE(current != nullptr);
  REQUIRE(current->get_current_hp() == 4);
  REQUIRE(current->get_max_hp() == 7);
}

} // namespace game
