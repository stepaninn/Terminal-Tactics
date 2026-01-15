#ifndef MYGAMEPROJECT_CONTROLLER_H
#define MYGAMEPROJECT_CONTROLLER_H

#include "model/service/AIService.h"
#include "model/service/CombatService.h"
#include "model/service/InventoryService.h"
#include "model/service/ItemService.h"
#include "model/service/MovementService.h"
#include "model/service/LoadSaveService.h"
#include "model/service/TurnService.h"
#include "model/service/World.h"
#include "types.h"

#include <string>
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
    PICK_ITEM,
    DROP_ITEM,
    USE_ITEM,
    NEXT_CELL_ITEM,
    PREV_CELL_ITEM,
    NEXT_INV_ITEM,
    PREV_INV_ITEM,
    RELOAD,
    END_TURN,
    TOGGLE_MODE,
    SAVE_GAME,
    LOAD_GAME,
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
               game::service::ItemService& items,
               game::service::InventoryService& inventory,
               game::service::AIService& ai);

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
    [[nodiscard]] bool quit_requested() const noexcept { return quit_requested_; }
    [[nodiscard]] size_t cell_item_index() const noexcept { return cell_item_index_; }
    [[nodiscard]] size_t inv_item_index() const noexcept { return inv_item_index_; }
    [[nodiscard]] bool save_game(const std::string& path) const;
    [[nodiscard]] bool load_game(const std::string& path);

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
    game::service::InventoryService& inventory_;
    game::service::AIService& ai_;
    game::service::LoadSaveService load_save_;

    game::Position cursor_{0, 0};
    game::EntityId selected_{game::service::TurnService::kNoEntity};
    Mode mode_{Mode::SELECT};
    game::TeamId player_team_{0};
    bool quit_requested_{false};
    size_t cell_item_index_{0};
    size_t inv_item_index_{0};
};

void set_io_path(std::string path);
std::string take_io_path();

}

#endif // MYGAMEPROJECT_CONTROLLER_H
