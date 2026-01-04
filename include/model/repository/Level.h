#ifndef INC_3_LEVEL_H
#define INC_3_LEVEL_H

#include "../../types.h"
#include "../entity/entities/Entity.h"
#include "cells/Cell.h"
#include "Matrix.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <ranges>

namespace game::repo {

using FieldMatrix = Matrix<std::unique_ptr<cells::ICell>>;

class Level {
public:
    Level() = delete;
    explicit Level(game::LevelId id, std::string name) : id_(id), name_(std::move(name)) {}

    [[nodiscard]] const FieldMatrix& get_field() const noexcept { return field_; }
    [[nodiscard]] FieldMatrix& get_field() noexcept { return field_; }

    [[nodiscard]] game::entity::Entity* get_entity(game::EntityId id) {
        auto it = entities_.find(id);
        return it != entities_.end() ? it->second.get() : nullptr;
    }
    [[nodiscard]] std::vector<const game::entity::Entity*> get_entities() noexcept;
    [[nodiscard]] const game::Position* get_entity_position(game::EntityId id) const noexcept;

    [[nodiscard]] cells::ICell* get_cell(game::Position pos) const noexcept;
    std::unique_ptr<cells::ICell> set_cell(game::Position pos, std::unique_ptr<cells::ICell> cell);

    void spawn_entity(std::unique_ptr<game::entity::Entity> e, game::Position pos);
    bool move_entity(game::EntityId id, game::Position to);

    std::unique_ptr<game::entity::Entity> remove_entity(const game::entity::Entity* e);
    std::unique_ptr<game::entity::Entity> remove_entity(game::EntityId id);

    [[nodiscard]] std::vector<const game::entity::Entity*> get_entities_radius(game::Position pos, int r) const;

    void resize_field(size_t rows, size_t cols) { field_.resize(rows, cols); }

    [[nodiscard]] game::LevelId get_id() const noexcept { return id_; }
    const std::string& get_name() const noexcept { return name_; }
    void set_name(const std::string& new_name) { name_ = new_name; }

private:
    game::LevelId id_;
    std::string name_;

    FieldMatrix field_;

    std::unordered_map<game::EntityId, std::unique_ptr<game::entity::Entity>> entities_;
    std::unordered_map<game::EntityId, game::Position> entity_positions_;

    game::EntityId next_entity_id_{1};
};

}

#endif //INC_3_LEVEL_H
