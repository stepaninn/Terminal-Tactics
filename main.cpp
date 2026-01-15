#include "controller/Controller.h"
#include "model/entity/components/AIComponent.h"
#include "model/entity/EntityFactory.h"
#include "model/entity/entities/items/Item.h"
#include "model/entity/entities/items/Weapon.h"
#include "model/repository/cells/Floor.h"
#include "model/service/AIService.h"
#include "model/service/CombatService.h"
#include "model/service/InventoryService.h"
#include "model/service/ItemService.h"
#include "model/service/MovementService.h"
#include "model/service/TurnService.h"
#include "model/service/World.h"
#include "model/service/events/EventBus.h"
#include "view/NcursesView.h"

#include <csignal>
#include <cstdlib>
#include <memory>
#include <ncurses.h>

#include "model/repository/cells/Glass.h"
#include "model/repository/cells/Partition.h"
#include "model/repository/cells/Stash.h"
#include "model/repository/cells/Wall.h"

namespace {
volatile std::sig_atomic_t g_should_quit = 0;

void on_signal(int) {
    g_should_quit = 1;
}

void restore_terminal() {
    endwin();
}
}

int main() {
    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);

    constexpr int kWidth = 40;
    constexpr int kHeight = 22;

    auto level = std::make_unique<game::repo::Level>(1, "Demo");
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

    auto set_wall = [&](int x, int y) {
        level->set_cell(game::Position{x, y}, std::make_unique<game::repo::cells::Wall>());
    };
    auto set_glass = [&](int x, int y) {
        level->set_cell(game::Position{x, y}, std::make_unique<game::repo::cells::Glass>());
    };
    auto set_partition = [&](int x, int y) {
        level->set_cell(game::Position{x, y}, std::make_unique<game::repo::cells::Partition>());
    };

    for (int x = 1; x < kWidth - 1; ++x) {
        if (x == 8 || x == 30) continue;
        set_wall(x, 7);
    }
    for (int x = 1; x < kWidth - 1; ++x) {
        if (x == 12 || x == 22) continue;
        set_wall(x, 15);
    }
    for (int y = 1; y < kHeight - 1; ++y) {
        if (y == 10) continue;
        set_wall(14, y);
    }
    for (int y = 1; y < kHeight - 1; ++y) {
        if (y == 12) continue;
        set_wall(25, y);
    }

    for (int x = 3; x < 12; ++x) set_partition(x, 4);
    for (int y = 9; y < 14; ++y) set_glass(18, y);
    for (int x = 28; x < 36; ++x) set_partition(x, 11);
    for (int y = 3; y < 6; ++y) set_glass(33, y);

    level->set_cell(game::Position{3, 3}, std::make_unique<game::repo::cells::Stash>());
    level->set_cell(game::Position{34, 18}, std::make_unique<game::repo::cells::Stash>());

    auto add_operative = [&](game::EntityId id, std::string name, game::Position pos, game::AmmoType ammo) {
        auto weapon = std::make_unique<game::entity::items::Weapon>(
            id * 10 + 1, 2, game::Damage{1, 4}, 6, 1, 2, ammo, 6, 8);
        auto ent = game::entity::factory::EntityFactory::create_operative(
            id, std::move(name), std::move(weapon));
        level->spawn_entity(std::move(ent), pos);
    };

    auto add_enemy = [&](game::EntityId id,
                         std::string name,
                         game::Position pos,
                         game::entity::components::AIBehavior behavior,
                         game::AmmoType ammo) {
        auto weapon = std::make_unique<game::entity::items::Weapon>(
            id * 10 + 2, 2, game::Damage{1, 3}, 5, 1, 2, ammo, 4, 6);
        std::unique_ptr<game::entity::Entity> ent;
        using game::entity::components::AIBehavior;
        switch (behavior) {
            case AIBehavior::WILD:
                ent = game::entity::factory::EntityFactory::create_wild(
                    id, std::move(name), std::move(weapon));
                break;
            case AIBehavior::INTELLIGENT:
                ent = game::entity::factory::EntityFactory::create_intelligent(
                    id, std::move(name), std::move(weapon));
                break;
            case AIBehavior::FORAGER:
                ent = game::entity::factory::EntityFactory::create_forager(
                    id, std::move(name), std::move(weapon));
                break;
            default:
                ent = game::entity::factory::EntityFactory::create_wild(
                    id, std::move(name), std::move(weapon));
                break;
        }
        level->spawn_entity(std::move(ent), pos);
    };

    add_operative(1, "Alpha", {3, 10}, game::AmmoType::PISTOL);
    add_operative(2, "Bravo", {5, 11}, game::AmmoType::RIFLE);
    add_operative(3, "Charlie", {4, 13}, game::AmmoType::SHOTGUN);

    add_enemy(10, "Raider", {32, 4}, game::entity::components::AIBehavior::WILD, game::AmmoType::SHOTGUN);
    add_enemy(11, "Sniper", {29, 17}, game::entity::components::AIBehavior::INTELLIGENT, game::AmmoType::SNIPER);
    add_enemy(12, "Scavenger", {20, 18}, game::entity::components::AIBehavior::FORAGER, game::AmmoType::PISTOL);

    if (auto* ent = level->get_entity(1)) {
        if (auto* inv = ent->get_component<game::entity::components::InventoryComponent>()) {
            inv->add(std::make_unique<game::entity::items::Medkit>(100, 2, 5, 2));
            inv->add(std::make_unique<game::entity::items::AmmoBag>(101, 1, 6, 12, game::AmmoType::PISTOL));
        }
    }
    if (auto* ent = level->get_entity(2)) {
        if (auto* inv = ent->get_component<game::entity::components::InventoryComponent>()) {
            inv->add(std::make_unique<game::entity::items::AmmoBag>(102, 1, 6, 12, game::AmmoType::RIFLE));
        }
    }
    if (auto* ent = level->get_entity(3)) {
        if (auto* inv = ent->get_component<game::entity::components::InventoryComponent>()) {
            inv->add(std::make_unique<game::entity::items::Medkit>(103, 2, 4, 2));
        }
    }

    level->add(game::Position{3, 3},
               std::make_unique<game::entity::items::Medkit>(200, 2, 4, 2));
    level->add(game::Position{34, 18},
               std::make_unique<game::entity::items::AmmoBag>(201, 1, 8, 12, game::AmmoType::PISTOL));
    level->add(game::Position{18, 10},
               std::make_unique<game::entity::items::AmmoBag>(202, 1, 4, 8, game::AmmoType::SHOTGUN));

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
    controller.set_cursor({1, 1});

    game::view::NcursesView view(bus);
    std::atexit(restore_terminal);
    bool running = true;
    while (running) {
        if (g_should_quit) break;
        view.render(world, controller);
        auto action = view.poll_input();
        running = controller.handle_action(action);
        bus->process();
    }
    return 0;
}
