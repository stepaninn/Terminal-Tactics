#include "controller/Controller.h"
#include "model/entity/EntityFactory.h"
#include "model/entity/entities/items/Item.h"
#include "model/entity/entities/items/Weapon.h"
#include "model/repository/cells/Floor.h"
#include "model/repository/cells/Stash.h"
#include "model/repository/cells/Wall.h"
#include "model/service/AIService.h"
#include "model/service/CombatService.h"
#include "model/service/InventoryService.h"
#include "model/service/ItemService.h"
#include "model/service/MovementService.h"
#include "model/service/TurnService.h"
#include "model/service/World.h"
#include "model/service/events/EventBus.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

class TimerGuard {
public:
    explicit TimerGuard(std::ostream& out, std::string label)
        : outStream_(out),
          label_(std::move(label)),
          start_(std::chrono::steady_clock::now()) {}

    ~TimerGuard() {
        const auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end - start_;
        outStream_ << label_ << ' ' << diff.count() << '\n';
        outStream_.flush();
    }

private:
    std::ostream& outStream_;
    std::string label_;
    std::chrono::steady_clock::time_point start_;
};

int main() {
    constexpr int kWidth = 100;
    constexpr int kHeight = 100;
    constexpr int kOperatives = 4;
    constexpr int kEnemies = 160;
    constexpr int kStashes = 15;
    constexpr int kFloorItems = 30;
    constexpr int kRuns = 10;
    constexpr const char* kBenchmarkPath = "benchmark.txt";

    std::mt19937 rng(std::random_device{}());
    std::ofstream bench_out(kBenchmarkPath, std::ios::app);

    auto random_ammo = [&rng]() {
        std::uniform_int_distribution<int> dist(0, 3);
        switch (dist(rng)) {
            case 0: return game::AmmoType::SHOTGUN;
            case 1: return game::AmmoType::RIFLE;
            case 2: return game::AmmoType::PISTOL;
            default: return game::AmmoType::SNIPER;
        }
    };

    auto make_level = [&]() {
        auto level = std::make_unique<game::repo::Level>(1, "RandomBox");
        level->resize_field(kWidth, kHeight);
        for (int x = 0; x < kWidth; ++x) {
            for (int y = 0; y < kHeight; ++y) {
                if (x == 0 || y == 0 || x == kWidth - 1 || y == kHeight - 1) {
                    level->set_cell(game::Position{x, y}, std::make_unique<game::repo::cells::Wall>());
                } else {
                    level->set_cell(game::Position{x, y}, std::make_unique<game::repo::cells::Floor>());
                }
            }
        }

        std::vector<game::Position> free_cells;
        free_cells.reserve(static_cast<size_t>((kWidth - 2) * (kHeight - 2)));
        for (int x = 1; x < kWidth - 1; ++x) {
            for (int y = 1; y < kHeight - 1; ++y) {
                free_cells.push_back({x, y});
            }
        }
        std::ranges::shuffle(free_cells, rng);

        auto take_cell = [&free_cells]() {
            game::Position pos = free_cells.back();
            free_cells.pop_back();
            return pos;
        };

        for (int i = 0; i < kStashes && !free_cells.empty(); ++i) {
            auto pos = take_cell();
            level->set_cell(pos, std::make_unique<game::repo::cells::Stash>());
        }

        game::ItemId item_id = 1000;
        for (int i = 0; i < kFloorItems && !free_cells.empty(); ++i) {
            auto pos = take_cell();
            std::unique_ptr<game::entity::items::Item> item;
            if (i % 3 == 0) {
                item = std::make_unique<game::entity::items::Medkit>(item_id++, 2, 4, 2);
            } else if (i % 3 == 1) {
                item = std::make_unique<game::entity::items::AmmoBag>(
                    item_id++, 1, 6, 12, random_ammo());
            } else {
                item = std::make_unique<game::entity::items::Weapon>(
                    item_id++, 2, game::Damage{1, 4}, 6, 1, 2, random_ammo(), 4, 6);
            }
            level->add(pos, std::move(item));
        }

        auto make_weapon = [&](game::EntityId id) {
            return std::make_unique<game::entity::items::Weapon>(
                id * 10 + 1, 2, game::Damage{1, 4}, 6, 1, 2, random_ammo(), 6, 8);
        };

        game::EntityId ent_id = 1;
        for (int i = 0; i < kOperatives && !free_cells.empty(); ++i, ++ent_id) {
            auto pos = take_cell();
            auto weapon = make_weapon(ent_id);
            auto ent = game::entity::factory::EntityFactory::create_operative(
                ent_id, "Operative_" + std::to_string(ent_id), std::move(weapon));
            level->spawn_entity(std::move(ent), pos);
        }

        for (int i = 0; i < kEnemies && !free_cells.empty(); ++i, ++ent_id) {
            auto pos = take_cell();
            auto weapon = make_weapon(ent_id);
            game::entity::components::AIBehavior behavior =
                (i % 3 == 0) ? game::entity::components::AIBehavior::WILD :
                (i % 3 == 1) ? game::entity::components::AIBehavior::INTELLIGENT :
                               game::entity::components::AIBehavior::FORAGER;
            std::unique_ptr<game::entity::Entity> ent;
            switch (behavior) {
                case game::entity::components::AIBehavior::WILD:
                    ent = game::entity::factory::EntityFactory::create_wild(
                        ent_id, "Enemy_" + std::to_string(ent_id), std::move(weapon));
                    break;
                case game::entity::components::AIBehavior::INTELLIGENT:
                    ent = game::entity::factory::EntityFactory::create_intelligent(
                        ent_id, "Enemy_" + std::to_string(ent_id), std::move(weapon));
                    break;
                case game::entity::components::AIBehavior::FORAGER:
                    ent = game::entity::factory::EntityFactory::create_forager(
                        ent_id, "Enemy_" + std::to_string(ent_id), std::move(weapon));
                    break;
            }
            level->spawn_entity(std::move(ent), pos);
        }
        return level;
    };

    for (int i = 0; i < kRuns; ++i) {
        auto level = make_level();
        auto bus = std::make_shared<game::service::events::EventBus>();

        game::service::World world(std::move(level));
        game::service::TurnService turn({0, 1});
        game::service::MovementService movement(bus);
        game::service::CombatService combat(bus);
        game::service::ItemService items(bus);
        game::service::InventoryService inventory(bus);
        game::service::VisionService vision;
        game::service::AIService ai(movement, combat, items, vision);

        game::controller::Controller controller(world, turn, movement, combat, items, inventory, ai);

        TimerGuard ai_timer(
            bench_out,
            "ai_turn w=" + std::to_string(kWidth) +
                " h=" + std::to_string(kHeight) +
                " ops=" + std::to_string(kOperatives) +
                " enemies=" + std::to_string(kEnemies) +
                " run=" + std::to_string(i + 1)
        );
        (void)controller.handle_action(game::controller::InputAction::END_TURN);
        // bus->process();
    }
    return 0;
}
