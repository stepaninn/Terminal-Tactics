#include "model/service/TurnService.h"
#include "model/entity/components/TimePointsComponent.h"

#include <ranges>
#include <utility>

namespace game::service {

void TurnService::set_teams(std::vector<game::TeamId> teams) { teams_ = std::move(teams); }

bool TurnService::select_entity(game::repo::Level& level, game::EntityId id) {
    auto* entity = level.get_entity(id);
    if (!entity) return false;
    if (!teams_.empty() && entity->get_team_id() != active_team_) return false;
    active_entity_ = entity->get_id();
    return true;
}

bool TurnService::end_entity_turn() noexcept {
    if (active_entity_ == kNoEntity) return false;
    active_entity_ = kNoEntity;
    return true;
}

bool TurnService::next_team(game::repo::Level& level) {
    if (teams_.empty()) return false;
    auto it = std::ranges::find(teams_, active_team_);
    if (it == teams_.end()) {
        active_team_ = teams_.front();
    } else {
        ++it;
        active_team_ = (it == teams_.end()) ? teams_.front() : *it;
    }
    active_entity_ = kNoEntity;
    refresh_team(level, active_team_);
    return true;
}

void TurnService::refresh_team(game::repo::Level& level, game::TeamId team_id) {
    auto entities = level.get_entities();
    for (auto* entity : entities) {
        if (!entity || entity->get_team_id() != team_id) continue;
        if (auto* tp = entity->get_component<game::entity::components::TimePointsComponent>()) {
            int missing = tp->get_max_points() - tp->get_current_points();
            if (missing > 0) tp->add_points(missing);
        }
    }
}

}
