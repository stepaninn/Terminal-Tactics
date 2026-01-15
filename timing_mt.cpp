#include "controller/Controller_mt.h"
#include "model/entity/EntityFactory_mt.h"
#include "model/entity/entities/items/Item_mt.h"
#include "model/entity/entities/items/Weapon_mt.h"
#include "model/repository/cells/Floor_mt.h"
#include "model/repository/cells/Stash_mt.h"
#include "model/repository/cells/Wall_mt.h"
#include "model/service/AIService_mt.h"
#include "model/service/CombatService_mt.h"
#include "model/service/InventoryService_mt.h"
#include "model/service/ItemService_mt.h"
#include "model/service/MovementService_mt.h"
#include "model/service/TurnService_mt.h"
#include "model/service/World_mt.h"
#include "model/service/events/EventBus_mt.h"

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
    constexpr int kRuns = 20;
    constexpr const char* kBenchmarkPath = "mt_benchmark.txt";

    std::mt19937 rng(std::random_device{}());
    std::ofstream bench_out(kBenchmarkPath, std::ios::app);

    auto random_ammo = [&rng]() {
        std::uniform_int_distribution<int> dist(0, 3);
        switch (dist(rng)) {
            case 0: return game::mt::AmmoType::SHOTGUN;
            case 1: return game::mt::AmmoType::RIFLE;
            case 2: return game::mt::AmmoType::PISTOL;
            default: return game::mt::AmmoType::SNIPER;
        }
    };

    auto make_level = [&]() {
        auto level = std::make_unique<game::mt::repo::Level>(1, "RandomBox");
        level->resize_field(kWidth, kHeight);
        for (int x = 0; x < kWidth; ++x) {
            for (int y = 0; y < kHeight; ++y) {
                if (x == 0 || y == 0 || x == kWidth - 1 || y == kHeight - 1) {
                    level->set_cell(game::mt::Position{x, y}, std::make_unique<game::mt::repo::cells::Wall>());
                } else {
                    level->set_cell(game::mt::Position{x, y}, std::make_unique<game::mt::repo::cells::Floor>());
                }
            }
        }

        std::vector<game::mt::Position> free_cells;
        free_cells.reserve(static_cast<size_t>((kWidth - 2) * (kHeight - 2)));
        for (int x = 1; x < kWidth - 1; ++x) {
            for (int y = 1; y < kHeight - 1; ++y) {
                free_cells.push_back({x, y});
            }
        }
        std::ranges::shuffle(free_cells, rng);

        auto take_cell = [&free_cells]() {
            game::mt::Position pos = free_cells.back();
            free_cells.pop_back();
            return pos;
        };

        for (int i = 0; i < kStashes && !free_cells.empty(); ++i) {
            auto pos = take_cell();
            level->set_cell(pos, std::make_unique<game::mt::repo::cells::Stash>());
        }

        game::mt::ItemId item_id = 1000;
        for (int i = 0; i < kFloorItems && !free_cells.empty(); ++i) {
            auto pos = take_cell();
            std::shared_ptr<game::mt::entity::items::Item> item;
            if (i % 3 == 0) {
                item = std::make_shared<game::mt::entity::items::Medkit>(item_id++, 2, 4, 2);
            } else if (i % 3 == 1) {
                item = std::make_shared<game::mt::entity::items::AmmoBag>(
                    item_id++, 1, 6, 12, random_ammo());
            } else {
                item = std::make_shared<game::mt::entity::items::Weapon>(
                    item_id++, 2, game::mt::Damage{1, 4}, 6, 1, 2, random_ammo(), 4, 6);
            }
            level->add(pos, std::move(item));
        }

        auto make_weapon = [&](game::mt::EntityId id) {
            return std::make_unique<game::mt::entity::items::Weapon>(
                id * 10 + 1, 2, game::mt::Damage{1, 4}, 6, 1, 2, random_ammo(), 6, 8);
        };

        game::mt::EntityId ent_id = 1;
        for (int i = 0; i < kOperatives && !free_cells.empty(); ++i, ++ent_id) {
            auto pos = take_cell();
            auto weapon = make_weapon(ent_id);
            auto ent = game::mt::entity::factory::EntityFactory::create_operative(
                ent_id, "Operative_" + std::to_string(ent_id), std::move(weapon));
            level->spawn_entity(std::move(ent), pos);
        }

        for (int i = 0; i < kEnemies && !free_cells.empty(); ++i, ++ent_id) {
            auto pos = take_cell();
            auto weapon = make_weapon(ent_id);
            game::mt::entity::components::AIBehavior behavior =
                (i % 3 == 0) ? game::mt::entity::components::AIBehavior::WILD :
                (i % 3 == 1) ? game::mt::entity::components::AIBehavior::INTELLIGENT :
                               game::mt::entity::components::AIBehavior::FORAGER;
            std::unique_ptr<game::mt::entity::Entity> ent;
            switch (behavior) {
                case game::mt::entity::components::AIBehavior::WILD:
                    ent = game::mt::entity::factory::EntityFactory::create_wild(
                        ent_id, "Enemy_" + std::to_string(ent_id), std::move(weapon));
                    break;
                case game::mt::entity::components::AIBehavior::INTELLIGENT:
                    ent = game::mt::entity::factory::EntityFactory::create_intelligent(
                        ent_id, "Enemy_" + std::to_string(ent_id), std::move(weapon));
                    break;
                case game::mt::entity::components::AIBehavior::FORAGER:
                    ent = game::mt::entity::factory::EntityFactory::create_forager(
                        ent_id, "Enemy_" + std::to_string(ent_id), std::move(weapon));
                    break;
            }
            level->spawn_entity(std::move(ent), pos);
        }
        return level;
    };

    for (int i = 0; i < kRuns; ++i) {
        auto level = make_level();
        auto bus = std::make_shared<game::mt::service::events::EventBus>();

        game::mt::service::World world(std::move(level));
        game::mt::service::TurnService turn({0, 1});
        game::mt::service::MovementService movement(bus);
        game::mt::service::CombatService combat(bus);
        game::mt::service::ItemService items(bus);
        game::mt::service::InventoryService inventory(bus);
        game::mt::service::VisionService vision;
        game::mt::service::AIService ai(movement, combat, items, vision);

        game::mt::controller::Controller controller(world, turn, movement, combat, items, inventory, ai);

        TimerGuard ai_timer(
            bench_out,
            "ai_turn w=" + std::to_string(kWidth) +
                " h=" + std::to_string(kHeight) +
                " ops=" + std::to_string(kOperatives) +
                " enemies=" + std::to_string(kEnemies) +
                " run=" + std::to_string(i + 1)
        );
        (void)controller.handle_action(game::mt::controller::InputAction::END_TURN);
        bus->process();
    }
    return 0;
}
