#ifndef INC_3_LEVEL_H
#define INC_3_LEVEL_H

#include "../../types.h"
#include "../entity/entities/Entity.h"
#include "cells/Cell.h"
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
     * @brief Метод получения клетки по позиции
     * @param pos позиция клетки
     * @return ICell* указатель на клетку или nullptr при выходе за границы
     */
    [[nodiscard]] cells::ICell* get_cell(game::Position pos) const noexcept;
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

private:
    game::LevelId id_;
    std::string name_;

    FieldMatrix field_;

    std::unordered_map<game::EntityId, std::unique_ptr<game::entity::Entity>> entities_;
    std::unordered_map<game::EntityId, game::Position> entity_positions_;

};

}

#endif //INC_3_LEVEL_H
