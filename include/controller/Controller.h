#ifndef MYGAMEPROJECT_CONTROLLER_H
#define MYGAMEPROJECT_CONTROLLER_H

#include "model/service/services.h"
#include "types.h"

#include <vector>

namespace game::controller {

enum class InputAction {
    NONE,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    CONFIRM,
    CANCEL,
    RELOAD,
    END_TURN,
    TOGGLE_MODE,
    QUIT
};

/// @brief Класс контроллер
class Controller {
public:
    enum class Mode {
        SELECT,
        MOVE,
        ATTACK
    };

    Controller(game::service::World& world,
               game::service::TurnService& turn,
               game::service::MovementService& move,
               game::service::CombatService& combat,
               game::service::ItemService& items);

    /**
     * @brief Метод обработки действия
     * @param action Тип действия
     * @return bool true, если действие успешно
     */
    [[nodiscard]] bool handle_action(InputAction action);

    /**
     * @brief Метод получения курсора на карте
     * @return Позиция курсора на карте
     */
    [[nodiscard]] game::Position get_cursor() const noexcept { return cursor_; }
    /**
     * @brief Метод установки курсора на позицию
     * @param pos Новая позиция курсора
     */
    void set_cursor(game::Position pos) noexcept { cursor_ = pos; }

    /**
     * @brief Метод получения ID выделенного существа
     * @return ID выбранного существа
     */
    [[nodiscard]] game::EntityId selected_entity() const noexcept { return selected_; }
    /**
     * @brief Метод получения режима курсора
     * @return Текущий режим курсора
     */
    [[nodiscard]] Mode get_mode() const noexcept { return mode_; }
    /**
     * @brief Метод получения ID текущей команды
     * @return ID теущей команды
     */
    [[nodiscard]] game::TeamId active_team() const noexcept { return turn_.active_team(); }
    /**
     * @brief Метод получения пути перемещения
     * @return Массив пути
     */
    [[nodiscard]] std::vector<game::Position> get_move_path() const;

private:
    void move_cursor(int dx, int dy);
    bool select_entity_at_cursor();
    bool move_selected_to_cursor();
    bool attack_at_cursor();
    void clear_selection() noexcept;

    game::service::World& world_;
    game::service::TurnService& turn_;
    game::service::MovementService& move_;
    game::service::CombatService& combat_;
    game::service::ItemService& items_;

    game::Position cursor_{0, 0};
    game::EntityId selected_{game::service::TurnService::kNoEntity};
    Mode mode_{Mode::SELECT};
};

}

#endif // MYGAMEPROJECT_CONTROLLER_H
