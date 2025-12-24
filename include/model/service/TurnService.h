#ifndef INC_3_TMP_SERVICE_TURNSERVICE_H
#define INC_3_TMP_SERVICE_TURNSERVICE_H

#include "model/repository/Level.h"

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

    void set_teams(std::vector<game::TeamId> teams);

    bool select_entity(game::repo::Level& level, game::EntityId id);

    bool end_entity_turn() noexcept;

    bool next_team(game::repo::Level& level);

    void refresh_team(game::repo::Level& level, game::TeamId team_id) const;

private:
    game::TeamId active_team_{0};
    game::EntityId active_entity_{kNoEntity};
    std::vector<game::TeamId> teams_;
};

}

#endif // INC_3_TMP_SERVICE_TURNSERVICE_H
