#include <catch2/catch_test_macros.hpp>

#include "model/entity/components/TimePointsComponent_mt.h"
#include "model/entity/entities/Entity_mt.h"
#include "model/repository/Level_mt.h"
#include "model/repository/cells/Floor_mt.h"
#include "model/repository/cells/Glass_mt.h"
#include "model/service/EntityLock_mt.h"
#include "model/service/TurnService_mt.h"

#include <array>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace game::mt {

TEST_CASE("multithreaded level replaces destructible cells without deadlock") {
    repo::Level level(1, "concurrency");
    level.resize_field(1, 1);
    level.set_cell({0, 0}, std::make_unique<repo::cells::Glass>());

    REQUIRE(level.try_shoot({0, 0}));
    REQUIRE(dynamic_cast<repo::cells::Floor*>(level.get_cell({0, 0})) != nullptr);
}

TEST_CASE("multithreaded level supports concurrent entity lookup") {
    repo::Level level(1, "concurrency");
    auto entity = std::make_unique<entity::Entity>(42, "unit", 1);
    REQUIRE(level.spawn_entity(std::move(entity), {3, 4}) != nullptr);

    std::atomic<bool> lookup_failed{false};
    std::array<std::jthread, 8> readers;
    for (auto& reader : readers) {
        reader = std::jthread([&level, &lookup_failed] {
            for (int i = 0; i < 1'000; ++i) {
                auto found = level.get_entity_at({3, 4});
                if (!found || found->get_id() != 42) lookup_failed.store(true);
            }
        });
    }
    for (auto& reader : readers) reader.join();

    REQUIRE_FALSE(lookup_failed.load());
}

TEST_CASE("time points component serializes concurrent updates") {
    entity::components::DefaultTimePointsComp points(0, 8'000);
    std::atomic<bool> update_failed{false};
    std::array<std::jthread, 8> workers;

    for (auto& worker : workers) {
        worker = std::jthread([&points, &update_failed] {
            for (int i = 0; i < 1'000; ++i) {
                if (points.add_points(1) != 1) update_failed.store(true);
            }
        });
    }
    for (auto& worker : workers) worker.join();

    REQUIRE_FALSE(update_failed.load());
    REQUIRE(points.get_current_points() == 8'000);
}

TEST_CASE("entity lock pool uses a stable order for pairs") {
    int protected_counter = 0;
    std::array<std::jthread, 8> workers;

    for (std::size_t index = 0; index < workers.size(); ++index) {
        workers[index] = std::jthread([index, &protected_counter] {
            for (int i = 0; i < 1'000; ++i) {
                [[maybe_unused]] auto locks = index % 2 == 0
                    ? service::EntityLockPool::instance().lock_entities(10, 20)
                    : service::EntityLockPool::instance().lock_entities(20, 10);
                ++protected_counter;
            }
        });
    }
    for (auto& worker : workers) worker.join();

    REQUIRE(protected_counter == 8'000);
}

TEST_CASE("turn refresh restores all members of a team in parallel") {
    repo::Level level(1, "concurrency");
    constexpr int entity_count = 128;

    for (int index = 0; index < entity_count; ++index) {
        auto unit = std::make_unique<entity::Entity>(index + 1, "unit", 1);
        unit->add_component<entity::components::TimePointsComponent,
                            entity::components::DefaultTimePointsComp>(0, 5);
        REQUIRE(level.spawn_entity(std::move(unit), {index, 0}) != nullptr);
    }

    service::TurnService::refresh_team(level, 1);

    for (const auto& unit : level.get_entities()) {
        auto* points = unit->get_component<entity::components::TimePointsComponent>();
        REQUIRE(points != nullptr);
        REQUIRE(points->get_current_points() == 5);
    }
}

} // namespace game::mt
