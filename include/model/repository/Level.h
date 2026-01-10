#ifndef MYGAMEPROJECT_LEVEL_H
#define MYGAMEPROJECT_LEVEL_H

#include "../../types.h"
#include "../entity/entities/Entity.h"
#include "cells/Cell.h"
#include "cells/DestructibleCell.h"
#include "cells/Floor.h"
#include "cells/ItemContainer.h"
#include "Matrix.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace game::repo {

using FieldMatrix = Matrix<std::unique_ptr<cells::ICell>>;

/// @brief Класс уровня с полем, сущностями и их позициями
class Level {
public:
    Level() = delete;
    explicit Level(game::LevelId id, std::string name) : id_(id), name_(std::move(name)) {}

    /**
     * @brief Метод получения поля уровня (только чтение)
     * @return const FieldMatrix& ссылка на поле
     */
    [[nodiscard]] const FieldMatrix& get_field() const noexcept { return field_; }
    /**
     * @brief Метод получения поля уровня
     * @return FieldMatrix& ссылка на поле
     */
    [[nodiscard]] FieldMatrix& get_field() noexcept { return field_; }

    /**
     * @brief Метод проверки наличия существа на уровне
     * @param id ID искомого существа
     * @return bool true, если существо есть на уровне
     */
    bool check_entity(game::EntityId id) const noexcept { return entities_.contains(id); }

    /**
     * @brief Метод получения сущности по идентификатору
     * @param id идентификатор сущности
     * @return Entity* указатель на сущность или nullptr
     */
    [[nodiscard]] game::entity::Entity* get_entity(game::EntityId id) {
        auto it = entities_.find(id);
        return it != entities_.end() ? it->second.get() : nullptr;
    }
    /**
     * @brief Метод получения всех сущностей уровня
     * @return std::vector<const Entity*> массив указателей на сущности
     */
    [[nodiscard]] std::vector<const game::entity::Entity*> get_entities() const noexcept;
    /**
     * @brief Метод получения позиции сущности
     * @param id идентификатор сущности
     * @return Position* указатель на позицию или nullptr
     */
    [[nodiscard]] const game::Position* get_entity_position(game::EntityId id) const noexcept;

    /**
     * @brief Метод получения сущности из позиции
     * @param pos Позиция, на которой мы ищем сущность
     * @return Entity* указатель если есть, nullptr иначе
     */
    [[nodiscard]] game::entity::Entity* get_entity_at(Position pos);

    /**
     * @brief Метод получения клетки по позиции
     * @param pos позиция клетки
     * @return ICell* указатель на клетку или nullptr при выходе за границы
     */
    [[nodiscard]] cells::ICell* get_cell(game::Position pos) const noexcept;
    /**
     * @brief Метод добавления предмета в клетку
     * @param pos позиция клетки
     * @param item предмет
     * @return unique_ptr на предмет при неуспехе, либо nullptr при успехе
     */
    std::unique_ptr<game::entity::items::Item> add(game::Position pos,
                                                   std::unique_ptr<game::entity::items::Item> item);
    /**
     * @brief Метод установки клетки по позиции
     * @param pos позиция клетки
     * @param cell новая клетка
     * @return unique_ptr на прежнюю клетку, либо возвращает переданную при выходе за границы
     */
    std::unique_ptr<cells::ICell> set_cell(game::Position pos, std::unique_ptr<cells::ICell> cell);

    /**
     * @brief Метод добавления сущности на уровень
     * @param e сущность
     * @param pos позиция сущности
     * @return Entity* указатель на добавленное существо
     * @note Идентификатор сущности должен быть уникальным
     */
    game::entity::Entity* spawn_entity(std::unique_ptr<game::entity::Entity> e, game::Position pos);
    /**
     * @brief Метод перемещения сущности
     * @param id идентификатор сущности
     * @param to новая позиция
     * @return bool true при успешном перемещении
     */
    bool move_entity(game::EntityId id, game::Position to);

    /**
     * @brief Метод удаления сущности по указателю
     * @param e указатель на сущность
     * @return unique_ptr на удаленную сущность или nullptr
     */
    std::unique_ptr<game::entity::Entity> remove_entity(const game::entity::Entity* e);
    /**
     * @brief Метод удаления сущности по идентификатору
     * @param id идентификатор сущности
     * @return unique_ptr на удаленную сущность или nullptr
     */
    std::unique_ptr<game::entity::Entity> remove_entity(game::EntityId id);

    /**
     * @brief Метод получения сущностей в радиусе от позиции
     * @param pos центр поиска
     * @param r радиус поиска (отрицательный радиус возвращает пустой список)
     * @return std::vector<const Entity*> массив указателей на сущности
     */
    [[nodiscard]] std::vector<const game::entity::Entity*> get_entities_radius(game::Position pos, int r) const;

    /**
     * @brief Метод изменения размера поля уровня
     * @param rows количество строк
     * @param cols количество столбцов
     */
    void resize_field(size_t rows, size_t cols) { field_.resize(rows, cols); }

    /**
     * @brief Метод получения идентификатора уровня
     * @return LevelId идентификатор уровня
     */
    [[nodiscard]] game::LevelId get_id() const noexcept { return id_; }
    /**
     * @brief Метод получения имени уровня
     * @return const std::string& имя уровня
     */
    const std::string& get_name() const noexcept { return name_; }
    /**
     * @brief Метод задания имени уровня
     * @param new_name новое имя
     */
    void set_name(const std::string& new_name) { name_ = new_name; }

    /**
     * @brief Метод получения ширины уровня
     * @return size_t ширина уровня
     */
    [[nodiscard]] size_t get_width() const noexcept { return field_.rows(); }
    /**
     * @brief Метод получения высоты уровня
     * @return size_t высота уровня
     */
    [[nodiscard]] size_t get_height() const noexcept { return field_.cols(); }

    /**
    * @brief Метод проверки вхождения позиции в границы уровня
     * @param pos Позиция
     * @return bool true, если позиция в границах уровня
     */
    [[nodiscard]] bool in_bounds(Position pos) const noexcept { return in_bounds(pos.x, pos.y); }
    /**
    * @brief Метод проверки вхождения координаты в границы уровня
     * @param x Координата по x
     * @param y Координата по y
     * @return bool true, если координата в границах уровня
     */
    [[nodiscard]] bool in_bounds(int x, int y) const noexcept {
        return x >= 0 && y >= 0 && static_cast<size_t>(x) < get_width()
            && static_cast<size_t>(y) < get_height();
    }

    /**
    * @brief Метод проверки блокирования обзора клеткой по позиции
    * @param pos Позиция
    * @return bool true, если клетка блокирует обзор
    */
    [[nodiscard]] bool is_blocks_vision(Position pos) const noexcept { return is_blocks_vision(pos.x, pos.y); }
    /**
    * @brief Метод проверки блокирования обзора клеткой по координатам
    * @param x Координата по x
    * @param y Координата по y
    * @return bool true, если клетка блокирует обзор
    */
    [[nodiscard]] bool is_blocks_vision(int x, int y) const noexcept {
        if (!in_bounds(x, y)) return true;
        return field_(static_cast<size_t>(x), static_cast<size_t>(y))->is_blocks_vision();
    }
    /**
     * @brief Метод проверки проходимости клетки по позиции
     * @param pos Позиция
     * @return bool true, если по клетке можно ходить
     */
    [[nodiscard]] bool is_walkable(Position pos) const noexcept { return is_walkable(pos.x, pos.y); }
    /**
     * @brief Метод проверки проходимости клетки по координатам
     * @param x Координата по x
     * @param y Координата по y
     * @return bool true, если по клетке можно ходить
     */
    [[nodiscard]] bool is_walkable(int x, int y) const noexcept {
        if (!in_bounds(x, y)) return false;
        return field_(static_cast<size_t>(x), static_cast<size_t>(y))->is_walkable();
    }
    /**
    * @brief Метод проверки прострела клетки по позиции
    * @param pos Позиция
    * @return bool true, если можно стрелять сквозь клетку
    */
    [[nodiscard]] bool can_shoot_through(Position pos) const noexcept { return can_shoot_through(pos.x, pos.y); }
    /**
    * @brief Метод проверки прострела клетки по координатам
    * @param x Координата по x
    * @param y Координата по y
    * @return bool true, если можно стрелять сквозь клетку
    */
    [[nodiscard]] bool can_shoot_through(int x, int y) const noexcept {
        if (!in_bounds(x, y)) return false;
        return operator()(x, y)->can_shoot_through();
    }

    /**
     * @brief Метод применения выстрела по клетке
     * @param pos позиция клетки
     * @return bool true, если состояние клетки изменилось
     */
    [[nodiscard]] bool try_shoot(Position pos) noexcept { return try_shoot(pos.x, pos.y); }

    /**
     * @brief Метод применения выстрела по клетке
     * @param x координата по x
     * @param y координата по y
     * @return bool true, если состояние клетки изменилось
     */
    [[nodiscard]] bool try_shoot(int x, int y) noexcept;

    /**
     * @brief Оператор доступа к клетке по координатам
     * @param x координата по x
     * @param y координата по y
     * @return ICell* указатель на клетку или nullptr при выходе за границы
     */
    game::repo::cells::ICell* operator() (int x, int y) const {
        if (!in_bounds(x, y)) return nullptr;
        return field_(static_cast<size_t>(x), static_cast<size_t>(y)).get();
    }

private:
    game::LevelId id_;
    std::string name_;

    FieldMatrix field_;

    std::unordered_map<game::EntityId, std::unique_ptr<game::entity::Entity>> entities_;
    std::unordered_map<game::EntityId, game::Position> entity_positions_;

};

}

#endif //MYGAMEPROJECT_LEVEL_H
