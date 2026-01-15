#include "model/service/TurnService_mt.h"
#include "model/entity/components/TimePointsComponent_mt.h"

#include <ranges>
#include <utility>

#include <tbb/parallel_for.h>

namespace game::mt::service {

void TurnService::set_teams(std::vector<game::mt::TeamId> teams) {
    std::lock_guard<std::mutex> lock(mutex_);
    teams_ = std::move(teams);
}

bool TurnService::select_entity(game::mt::repo::Level& level, game::mt::EntityId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entity = level.get_entity(id);
    if (!entity) return false;
    if (!teams_.empty() && entity->get_team_id() != active_team_) return false;
    active_entity_ = entity->get_id();
    return true;
}

bool TurnService::end_entity_turn() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (active_entity_ == kNoEntity) return false;
    active_entity_ = kNoEntity;
    return true;
}

bool TurnService::next_team(game::mt::repo::Level& level) {
    game::mt::TeamId next_team = 0;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (teams_.empty()) return false;
        auto it = std::ranges::find(teams_, active_team_);
        if (it == teams_.end()) {
            next_team = teams_.front();
        } else {
            ++it;
            next_team = (it == teams_.end()) ? teams_.front() : *it;
        }
        active_team_ = next_team;
        active_entity_ = kNoEntity;
    }
    refresh_team(level, next_team);
    return true;
}

void TurnService::refresh_team(game::mt::repo::Level& level, game::mt::TeamId team_id) {
    auto entities = level.get_entities();
    tbb::parallel_for(size_t{0}, entities.size(), [&](size_t i) {
        const auto& entity = entities[i];
        if (!entity || entity->get_team_id() != team_id) return;
        if (auto* tp = entity->get_component<game::mt::entity::components::TimePointsComponent>()) {
            int missing = tp->get_max_points() - tp->get_current_points();
            if (missing > 0) tp->add_points(missing);
        }
    });
}

}
