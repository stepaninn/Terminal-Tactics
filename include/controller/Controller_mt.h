#ifndef MYGAMEPROJECT_CONTROLLER_MT_H
#define MYGAMEPROJECT_CONTROLLER_MT_H

#include "model/service/AIService_mt.h"
#include "model/service/CombatService_mt.h"
#include "model/service/InventoryService_mt.h"
#include "model/service/ItemService_mt.h"
#include "model/service/MovementService_mt.h"
#include "model/service/LoadSaveService_mt.h"
#include "model/service/TurnService_mt.h"
#include "model/service/World_mt.h"
#include "types_mt.h"

#include <string>
#include <vector>

namespace game::mt::controller {

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

    Controller(game::mt::service::World& world,
               game::mt::service::TurnService& turn,
               game::mt::service::MovementService& move,
               game::mt::service::CombatService& combat,
               game::mt::service::ItemService& items,
               game::mt::service::InventoryService& inventory,
               game::mt::service::AIService& ai);

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
    [[nodiscard]] game::mt::Position get_cursor() const noexcept { return cursor_; }
    /**
     * @brief Метод установки курсора на позицию
     * @param pos Новая позиция курсора
     */
    void set_cursor(game::mt::Position pos) noexcept { cursor_ = pos; }

    /**
     * @brief Метод получения ID выделенного существа
     * @return ID выбранного существа
     */
    [[nodiscard]] game::mt::EntityId selected_entity() const noexcept { return selected_; }
    /**
     * @brief Метод получения режима курсора
     * @return Текущий режим курсора
     */
    [[nodiscard]] Mode get_mode() const noexcept { return mode_; }
    /**
     * @brief Метод получения ID текущей команды
     * @return ID теущей команды
     */
    [[nodiscard]] game::mt::TeamId active_team() const noexcept { return turn_.active_team(); }
    /**
     * @brief Метод получения пути перемещения
     * @return Массив пути
     */
    [[nodiscard]] std::vector<game::mt::Position> get_move_path() const;
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

    game::mt::service::World& world_;
    game::mt::service::TurnService& turn_;
    game::mt::service::MovementService& move_;
    game::mt::service::CombatService& combat_;
    game::mt::service::ItemService& items_;
    game::mt::service::InventoryService& inventory_;
    game::mt::service::AIService& ai_;
    game::mt::service::LoadSaveService load_save_;

    game::mt::Position cursor_{0, 0};
    game::mt::EntityId selected_{game::mt::service::TurnService::kNoEntity};
    Mode mode_{Mode::SELECT};
    game::mt::TeamId player_team_{0};
    bool quit_requested_{false};
    size_t cell_item_index_{0};
    size_t inv_item_index_{0};
};

void set_io_path(std::string path);
std::string take_io_path();

}

#endif // MYGAMEPROJECT_CONTROLLER_MT_H
