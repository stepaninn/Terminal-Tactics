#ifndef MYGAMEPROJECT_AISERVICE_H
#define MYGAMEPROJECT_AISERVICE_H

#include "AIBehavior.h"
#include "MovementService.h"
#include "CombatService.h"
#include "ItemService.h"
#include "VisionService.h"
#include "World.h"

#include "model/entity/components/AIComponent.h"

#include <optional>
#include <random>
#include <vector>

namespace game::service {

/// @brief Сервис ИИ, выбирает лучшее дейсвтие на основе очков
class AIService {
public:
    AIService(MovementService& move,
                     CombatService& combat,
                     ItemService& items,
                     VisionService& vision)
    : ctx_{move, combat, items, vision},
      rng_(std::random_device{}()) {}

    void act_entity(World& w, EntityId id);
    void act_team(World& w, TeamId team_id);

private:
    struct Target {
        EntityId id{0};
        Position pos{};
        int dist{0};
        bool line_of_fire{false};
    };

    enum class ActionKind {
        SHOOT,
        MELEE,
        PICKUPHERE,
        DEPOSITHERE,
        MOVETOENEMY,
        MOVETOITEM,
        MOVETOSTASH,
        WANDER,
        WAIT
    };

    struct Choice {
        ActionKind kind{ActionKind::WAIT};
        double score = 0;

        std::optional<Target> enemy;
        std::optional<Position> move_to;
    };

    Choice choose_best_action(World& w, EntityId id);

    void execute(World& w, EntityId id, const Choice& c);

    [[nodiscard]] static int get_distance(Position a, Position b);
    [[nodiscard]] static bool is_adjacent(Position a, Position b);

    [[nodiscard]] static double weight(entity::components::AIBehavior profile, ActionKind kind) ;

    [[nodiscard]] std::optional<Target> find_nearest_enemy(World& w, EntityId observer_id) const;
    [[nodiscard]] static std::vector<Position> find_stashes(const game::repo::Level& level) ;
    [[nodiscard]] static std::vector<Position> find_item_cells(const game::repo::Level& level) ;

    [[nodiscard]] bool move_one_step(World& w, EntityId id, Position to) const;
    [[nodiscard]] bool random_step(World& w, EntityId id);

    [[nodiscard]] static bool pickup_one_here(game::repo::Level& level, game::EntityId id);
    [[nodiscard]] static bool deposit_all_here(game::repo::Level& level, game::EntityId id);

    [[nodiscard]] static bool drop_all_to_cell(game::repo::cells::IItemContainer& cell,
                                        entity::components::InventoryComponent& inv);

    AIContext ctx_;
    std::mt19937 rng_;
};

}

#endif // MYGAMEPROJECT_AISERVICE_H
