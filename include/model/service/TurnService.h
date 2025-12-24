#ifndef INC_3_TMP_SERVICE_TURNSERVICE_H
#define INC_3_TMP_SERVICE_TURNSERVICE_H

#include "../repository/Level.h"
#include "../entity/components/TimePointsComponent.h"

#include <algorithm>
#include <vector>

namespace game::service {

class TurnService {
public:
    static constexpr game::EntityId kNoEntity = 0;

    TurnService() : teams_{0, 1} {}
    explicit TurnService(std::vector<game::TeamId> teams) : teams_(std::move(teams)) {}

    [[nodiscard]] game::TeamId active_team() const noexcept { return active_team_; }
    [[nodiscard]] game::EntityId active_entity() const noexcept { return active_entity_; }

    void set_active_team(game::TeamId team) noexcept { active_team_ = team; }

    void set_teams(std::vector<game::TeamId> teams) { teams_ = std::move(teams); }

    bool select_entity(game::repo::Level& level, game::EntityId id) {
        auto* entity = level.get_entity(id);
        if (!entity) return false;
        if (!teams_.empty() && entity->get_team_id() != active_team_) return false;
        active_entity_ = id;
        return true;
    }

    bool end_entity_turn() noexcept {
        if (active_entity_ == kNoEntity) return false;
        active_entity_ = kNoEntity;
        return true;
    }

    bool next_team(game::repo::Level& level) {
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

    void refresh_team(game::repo::Level& level, game::TeamId team_id) const {
        auto entities = level.get_entities();
        for (auto* entity : entities) {
            if (!entity || entity->get_team_id() != team_id) continue;
            if (auto* tp = entity->get_component<game::entity::components::TimePointsComponent>()) {
                int missing = tp->get_max_points() - tp->get_current_points();
                if (missing > 0) tp->add_points(missing);
            }
        }
    }

private:
    game::TeamId active_team_{0};
    game::EntityId active_entity_{kNoEntity};
    std::vector<game::TeamId> teams_;
};

}

#endif // INC_3_TMP_SERVICE_TURNSERVICE_H
