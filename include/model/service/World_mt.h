#ifndef MYGAMEPROJECT_WORLD_MT_H
#define MYGAMEPROJECT_WORLD_MT_H

#include "model/repository/Level_mt.h"
#include "events/EventBus_mt.h"
#include "Query_mt.h"
#include "Matrix_mt.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <random>
#include <set>
#include <tbb/concurrent_hash_map.h>
#include <shared_mutex>
#include <vector>

namespace game::mt::service {

/// @brief Класс карты видимости
class VisibilityMap {
public:
    VisibilityMap() = default;
    VisibilityMap(size_t rows, size_t cols) : cells_(rows, cols, 0) {}

    /**
     * @brief Метод изменения размера карты видимости
     * @param rows Количество строк
     * @param cols Количество рядов
     */
    void resize(size_t rows, size_t cols) { cells_.resize(rows, cols); }

    /**
     * @brief Метод очистки карты видимости
     */
    void clear_all() { std::ranges::fill(cells_, 0); }

    /**
     * @brief Метод очистки видимых клеток
     */
    void clear_visible() {
        for (auto& cell : cells_) cell &= ~kVisible;
    }

    /**
     * @brief Метод установки видимости координаты
     * @param x Координата x
     * @param y Координата y
     * @param visible Флаг видимости клетки
     */
    void set_visible(size_t x, size_t y, bool visible = true) {
        if (!in_bounds(x, y)) return;
        auto& cell = cells_(x, y);
        if (visible) cell |= static_cast<uint8_t>(kVisible) | static_cast<uint8_t>(kExplored);
        else cell &= static_cast<uint8_t>(~kVisible);
    }
    /**
     * @brief Метод установки видимости позиции
     * @param pos Позиция
     * @param visible Флаг видимости клетки
     */
    void set_visible(game::mt::Position pos, bool visible = true) {
        if (!in_bounds(pos)) return;
        set_visible(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y), visible);
    }
    /**
     * @brief Метод установки флага исследованности клетки
     * @param x Координата x
     * @param y Координата y
     * @param explored Флаг исследованности клетки
     */
    void set_explored(size_t x, size_t y, bool explored = true) {
        if (!in_bounds(x, y)) return;
        auto& cell = cells_(x, y);
        if (explored) cell |= static_cast<uint8_t>(kExplored);
        else cell &= static_cast<uint8_t>(~kExplored);
    }
    /**
     * @brief Метод установки флага исследованности позиции
     * @param pos Позиция
     * @param explored Флаг исследованности клетки
     */
    void set_explored(game::mt::Position pos, bool explored = true) {
        if (!in_bounds(pos)) return;
        set_explored(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y), explored);
    }

    /**
     * @brief Метод проверки видимости клетки
     * @param x Координата x
     * @param y Координата y
     * @return bool true, если видима
     */
    [[nodiscard]] bool is_visible(size_t x, size_t y) const {
        return in_bounds(x, y) && (cells_(x, y) & kVisible) != 0;
    }
    /**
     * @brief Метод проверки видимости клетки по позиции
     * @param pos Позиция
     * @return bool true, если видима
     */
    [[nodiscard]] bool is_visible(game::mt::Position pos) const {
        return in_bounds(pos) && (cells_(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)) & kVisible) != 0;
    }

    /**
     * @brief Метод проверки была ли клетка хоть раз исследована
     * @param x Координата x
     * @param y Координата y
     * @return bool true, если было исследовано
     */
    [[nodiscard]] bool is_explored(size_t x, size_t y) const {
        return in_bounds(x, y) && (cells_(x, y) & kExplored) != 0;
    }
    /**
     * @brief Метод проверки была ли клетка хоть раз исследована по позиции
     * @param pos Позиция
     * @return bool true, если было исследовано
     */
    [[nodiscard]] bool is_explored(game::mt::Position pos) const {
        return in_bounds(pos) && (cells_(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)) & kExplored) != 0;
    }

    [[nodiscard]] size_t rows() const noexcept { return cells_.rows(); }
    [[nodiscard]] size_t cols() const noexcept { return cells_.cols(); }

private:
    static constexpr uint8_t kVisible = 1u << 0;  // 0000 0001
    static constexpr uint8_t kExplored = 1u << 1; // 0000 0010

    [[nodiscard]] bool in_bounds(size_t x, size_t y) const {
        return x < cells_.rows() && y < cells_.cols();
    }
    /**
     * @brief Метод проверки вхождения позиции в границы карты видимости
     * @param pos Позиция
     * @return bool true, если позиция в границах карты
     */
    [[nodiscard]] bool in_bounds(game::mt::Position pos) const {
        return pos.x >= 0 && pos.y >= 0
            && static_cast<size_t>(pos.x) < cells_.rows()
            && static_cast<size_t>(pos.y) < cells_.cols();
    }

    Matrix<uint8_t> cells_;
};

/// @brief Класс мира
class World {
public:
    explicit World(std::unique_ptr<game::mt::repo::Level> level, std::unique_ptr<events::EventBus> bus = nullptr)
        : level_(std::move(level)), event_bus_(std::move(bus)) { make_teams(); }

    /**
     * @brief Метод, возвращающий текущий уровень
     * @return Текущий уровень
     */
    [[nodiscard]] game::mt::repo::Level* get_level() const noexcept { return level_.get(); }
    /**
     * @brief Метод задания уровня
     * @param level Новый уровень
     */
    void set_level(std::unique_ptr<game::mt::repo::Level> level);
    /**
     * @brief Метод удаления и получения текущего уровня
     * @return unique_ptr указатель на уровень
     */
    std::unique_ptr<game::mt::repo::Level> take_level();

    /**
     * @brief Метод получения видимых клеток для существа
     * @param entity_id ID существа
     * @return VisibilityMap* карта видимости для существа
     */
    [[nodiscard]] const VisibilityMap* unit_visibility(game::mt::EntityId entity_id) const {
        std::shared_lock<std::shared_mutex> lock(fov_mutex_);
        tbb::concurrent_hash_map<game::mt::EntityId, std::unique_ptr<game::mt::service::VisibilityMap>>::const_accessor acc;
        if (unit_fov_.find(acc, entity_id)) return acc->second.get();
        return nullptr;
    }
    [[nodiscard]] VisibilityMap* unit_visibility(game::mt::EntityId entity_id);

    /**
     * @brief Метод получения видимых клеток для команды
     * @param team_id ID команды
     * @return VisibilityMap* карта видимости для команды
     */
    [[nodiscard]] const VisibilityMap* team_visibility(game::mt::TeamId team_id) const {
        std::shared_lock<std::shared_mutex> lock(fov_mutex_);
        tbb::concurrent_hash_map<game::mt::TeamId, std::unique_ptr<game::mt::service::VisibilityMap>>::const_accessor acc;
        if (team_visible_.find(acc, team_id)) return acc->second.get();
        return nullptr;
    }
    [[nodiscard]] VisibilityMap* team_visibility(game::mt::TeamId team_id);

    /**
     * @brief Метод получения исследованных клеток для команды
     * @param team_id ID команды
     * @return VisibilityMap* карта видимости для команды
     */
    [[nodiscard]] const VisibilityMap* team_exploration(game::mt::TeamId team_id) const {
        std::shared_lock<std::shared_mutex> lock(fov_mutex_);
        tbb::concurrent_hash_map<game::mt::TeamId, std::unique_ptr<game::mt::service::VisibilityMap>>::const_accessor acc;
        if (team_explored_.find(acc, team_id)) return acc->second.get();
        return nullptr;
    }
    [[nodiscard]] VisibilityMap* team_exploration(game::mt::TeamId team_id);

    /**
     * @brief Метод изменения размера карты
     * @param rows Количество строк
     * @param cols Количество рядов
     */
    void resize(size_t rows, size_t cols);

    /**
     * @brief Метод, возвращающий текущий EventBus
     * @return Текущий EventBus
     */
    [[nodiscard]] events::EventBus* get_event_bus() const noexcept { return event_bus_.get(); }

    /**
     * @brief Метод поиска существа по ID на текущем уровне
     * @param id ID искомого существа
     * @return Указатель на искомое существо
     */
    [[nodiscard]] std::shared_ptr<game::mt::entity::Entity> get_entity(game::mt::EntityId id) const {
        return level_ ? level_->get_entity(id) : nullptr;
    }

    /**
     * @brief Метод получения существ на текущем уровне
     * @return Массив существ на текущем уровне
     */
    [[nodiscard]] std::vector<std::shared_ptr<const game::mt::entity::Entity>> get_all_entities() const {
        if (!level_) return {};
        return level_->get_entities();
    }

    /**
     * @brief Метод получения списка ID существ по команде
     * @param team_id ID команды
     * @return Массив ID существ команды
     */
    [[nodiscard]] std::vector<game::mt::EntityId> get_team_entities(game::mt::TeamId team_id) const {
        std::shared_lock<std::shared_mutex> lock(teams_mutex_);
        tbb::concurrent_hash_map<game::mt::TeamId, std::set<game::mt::EntityId>>::const_accessor acc;
        if (teams_.find(acc, team_id)) {
            return {acc->second.begin(), acc->second.end()};
        }
        return {};
    }

    /**
     * @brief Метод добавления сущности на уровень
     * @param e сущность
     * @param pos позиция сущности
     * @return Entity* указатель на добавленное существо
     * @note Идентификатор сущности должен быть уникальным
     */
    std::shared_ptr<game::mt::entity::Entity> spawn_entity(std::unique_ptr<game::mt::entity::Entity> e,
                                                           game::mt::Position pos);

    /**
     * @brief Метод перемещения сущности
     * @param id идентификатор сущности
     * @param to новая позиция
     * @return bool true при успешном перемещении
     */
    bool move_entity(game::mt::EntityId id, game::mt::Position to) {
        return level_ ? level_->move_entity(id, to) : false;
    }

    /**
     * @brief Метод удаления сущности по указателю
     * @param e указатель на сущность
     * @return unique_ptr на удаленную сущность или nullptr
     */
    std::shared_ptr<game::mt::entity::Entity> remove_entity(const game::mt::entity::Entity* e);

    /**
     * @brief Метод удаления сущности по идентификатору
     * @param id идентификатор сущности
     * @return unique_ptr на удаленную сущность или nullptr
     */
    std::shared_ptr<game::mt::entity::Entity> remove_entity(game::mt::EntityId id);

    /**
     * @brief Метод получения ширины уровня
     * @return size_t ширина уровня
     */
    size_t get_width() const noexcept { return level_->get_width(); }
    /**
     * @brief Метод получения высоты уровня
     * @return size_t высота уровня
     */
    size_t get_height() const noexcept { return level_->get_height(); }

    /**
     * @brief Метод смены команды сущности
     * @param id идентификатор сущности
     * @param team_id новая команда
     * @return bool true при успешной смене команды
     */
    bool set_entity_team(game::mt::EntityId id, game::mt::TeamId team_id);

    /**
     * @brief Метод, возвращаюший существ по компоненту
     * @tparam Comps Типы компонентов, по которым осуществляется поиск
     * @return Массив существ, которые имеют данный компонент
     */
    template<typename... Comps>
    [[nodiscard]] std::vector<std::shared_ptr<const game::mt::entity::Entity>> view() const {
        std::vector<std::shared_ptr<const game::mt::entity::Entity>> res;
        if (!level_) return res;
        auto all = level_->get_entities();
        res.reserve(all.size());
        for (const auto& entity : all) {
            if (entity && game::mt::service::has_components<Comps...>(*entity)) res.push_back(entity);
        }
        return res;
    }

private:
    std::unique_ptr<game::mt::repo::Level> level_;
    std::unique_ptr<events::EventBus> event_bus_;

    mutable std::shared_mutex fov_mutex_;
    tbb::concurrent_hash_map<game::mt::EntityId, std::unique_ptr<game::mt::service::VisibilityMap>> unit_fov_;
    tbb::concurrent_hash_map<game::mt::TeamId, std::unique_ptr<game::mt::service::VisibilityMap>> team_visible_;
    tbb::concurrent_hash_map<game::mt::TeamId, std::unique_ptr<game::mt::service::VisibilityMap>> team_explored_;

    void make_teams();

    void remove_from_team(game::mt::TeamId team_id, game::mt::EntityId id);

    mutable std::shared_mutex teams_mutex_;
    tbb::concurrent_hash_map<game::mt::TeamId, std::set<game::mt::EntityId>> teams_;
};

}

#endif // MYGAMEPROJECT_WORLD_MT_H
