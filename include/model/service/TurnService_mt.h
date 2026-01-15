#ifndef MYGAMEPROJECT_TURNSERVICE_MT_H
#define MYGAMEPROJECT_TURNSERVICE_MT_H

#include "model/repository/Level_mt.h"
#include "types_mt.h"

#include <algorithm>
#include <mutex>
#include <vector>

namespace game::mt::service {

/// @brief Сервис смены ходов
class TurnService {
public:
    static constexpr game::mt::EntityId kNoEntity = 0;

    TurnService() : teams_{0, 1} {}
    explicit TurnService(std::vector<game::mt::TeamId> teams) : teams_(std::move(teams)) {}

    /**
     * @brief Метод, показывающий какая команда ходит в данный момент
     * @return ID активной команды
     */
    [[nodiscard]] game::mt::TeamId active_team() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return active_team_;
    }
    /**
     * @brief Метод, показывающий, какое существо ходит в данный момент
     * @return ID активного существа
     */
    [[nodiscard]] game::mt::EntityId active_entity() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return active_entity_;
    }

    /**
     * @brief Метод, который задает активную команду
     * @param team ID новой активной команды
     */
    void set_active_team(game::mt::TeamId team) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        active_team_ = team;
    }

    /**
     * @brief Метод, который задает активные команды
     * @param teams ID команд, которые участвуют в игре
     */
    void set_teams(std::vector<game::mt::TeamId> teams);

    /**
     * @brief Метод, выбирающий активное существо для хода
     * @param level Уровень, на котором выбирается активное существо
     * @param id ID выбираемого существа
     * @return bool true, если выбор успешен
     */
    [[nodiscard]] bool select_entity(game::mt::repo::Level& level, game::mt::EntityId id);

    /**
     * @brief Метод досрочного самостоятельного окончания хода
     * @return bool true, если ход закончен, false если существо не выбрано
     */
    [[nodiscard]] bool end_entity_turn() noexcept;

    /**
     * @brief Метод смены действующей команды
     * @param level Уровень, на котором выбирается следующая команда
     * @return bool true, если смена команды успешна
     */
    [[nodiscard]] bool next_team(game::mt::repo::Level& level);

    /**
     * @brief Метод, обновляющий очки действия каждому члену команды
     * @param level Уровень, на котором обновляются очки действия команды
     * @param team_id Команда, для которой обновляются очки действия
     */
    static void refresh_team(game::mt::repo::Level& level, game::mt::TeamId team_id);

    /**
     * @brief Метод получения списка команд
     * @return Массив ID команд
     */
    [[nodiscard]] std::vector<game::mt::TeamId> teams() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return teams_;
    }

private:
    mutable std::mutex mutex_;
    game::mt::TeamId active_team_{0};
    game::mt::EntityId active_entity_{kNoEntity};
    std::vector<game::mt::TeamId> teams_;
};

}

#endif // MYGAMEPROJECT_TURNSERVICE_MT_H
