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

    /**
     * @brief Метод, показывающий какая команда ходит в данный момент
     * @return ID активной команды
     */
    [[nodiscard]] game::TeamId active_team() const noexcept { return active_team_; }
    /**
     * @brief Метод, показывающий, какое существо ходит в данный момент
     * @return ID активного существа
     */
    [[nodiscard]] game::EntityId active_entity() const noexcept { return active_entity_; }

    /**
     * @brief Метод, который задает активную команду
     * @param team ID новой активной команды
     */
    void set_active_team(game::TeamId team) noexcept { active_team_ = team; }

    /**
     * @brief Метод, который задает активные команды
     * @param teams ID команд, которые участвуют в игре
     */
    void set_teams(std::vector<game::TeamId> teams);

    /**
     * @brief Метод, выбирающий активное существо для хода
     * @param level Уровень, на котором выбирается активное существо
     * @param id ID выбираемого существа
     * @return bool true, если выбор успешен
     */
    bool select_entity(game::repo::Level& level, game::EntityId id);

    /**
     * @brief Метод досрочного самостоятельного окончания хода
     * @return bool true, если ход закончен, false если существо не выбрано
     */
    bool end_entity_turn() noexcept;

    /**
     * @brief Метод смены действующей команды
     * @param level Уровень, на котором выбирается следующая команда
     * @return bool true, если смена команды успешна
     */
    bool next_team(game::repo::Level& level);

    /**
     * @brief Метод, обновляющий очки действия каждому члену команды
     * @param level Уровень, на котором обновляются очки действия команды
     * @param team_id Команда, для которой обновляются очки действия
     */
    void refresh_team(game::repo::Level& level, game::TeamId team_id) const;

private:
    game::TeamId active_team_{0};
    game::EntityId active_entity_{kNoEntity};
    std::vector<game::TeamId> teams_;
};

}

#endif // INC_3_TMP_SERVICE_TURNSERVICE_H
