#ifndef INC_3_LEVEL_H
#define INC_3_LEVEL_H

#include "../../types.h"
#include "../entity/entities/Entity.h"
#include "cells/Cell.h"
#include "Matrix.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace game {

using FieldMatrix = Matrix<std::unique_ptr<ICell>>;

class Level {
public:
    Level() = delete;
    explicit Level(LevelId id, std::string name);

    [[nodiscard]] const FieldMatrix& get_field() const noexcept { return field_; }
    [[nodiscard]] FieldMatrix& get_field() noexcept { return field_; }

    [[nodiscard]] Entity* get_entity(EntityId id) { return entities_[id].get(); }
    [[nodiscard]] std::vector<const Entity*> get_entities() noexcept;

    [[nodiscard]] ICell* get_cell(Position pos) const noexcept;
    std::unique_ptr<ICell> set_cell(Position pos, std::unique_ptr<ICell> cell);

    void spawn_entity(std::unique_ptr<Entity> e, Position pos);

    std::unique_ptr<Entity> remove_entity(const Entity* e);
    std::unique_ptr<Entity> remove_entity(EntityId id);

    [[nodiscard]] std::vector<const Entity*> get_entities_radius(Position pos, int r) const;

    void resize_field(size_t rows, size_t cols) { field_.resize(rows, cols); }

    [[nodiscard]] LevelId get_id() const noexcept { return id_; }
    const std::string& get_name() const noexcept { return name_; }
    void set_name(const std::string& new_name) { name_ = new_name; }

private:
    LevelId id_;
    std::string name_;

    FieldMatrix field_;

    std::unordered_map<EntityId, std::unique_ptr<Entity>> entities_;
    std::unordered_map<EntityId, Position> entity_positions_;

    EntityId next_entity_id_{1};
};

} // namespace game

#endif //INC_3_LEVEL_H