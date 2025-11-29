#ifndef INC_3_LEVEL_H
#define INC_3_LEVEL_H

#include "types.hpp"
#include "Cell.hpp"
#include <map>
#include <vector>
#include <memory>

namespace game {

using FieldMatrix = Matrix<std::shared_ptr<Cell>>;

class Level {
public:
    Level() = default;
    explicit Level(id_t id, std::string name);

    const FieldMatrix& get_field() const noexcept { return field_; }
    FieldMatrix& get_field() noexcept { return field_; }

    const std::map<id_t, std::shared_ptr<Entity>>& get_entities() const noexcept { return entities_; }
    std::map<id_t, std::shared_ptr<Entity>>& get_entities() noexcept { return entities_; }

    std::weak_ptr<Cell> get_cell(const Position& pos) const noexcept;

    bool set_cell(const Position& pos, std::shared_ptr<Cell> cell);

    bool spawn_entity(std::shared_ptr<Entity> e, const Position& pos);

    const std::shared_ptr<Entity>& remove_entity(const std::shared_ptr<Entity>& e);

    std::vector<std::shared_ptr<Entity>> get_entities_radius(const Position& pos, int r) const;

    bool resize_field(std::size_t rows, std::size_t cols);

    id_t get_id() const noexcept { return id_; }
    const std::string& get_name() const noexcept { return name_; }
    void set_name(std::string new_name) { name_ = new_name; }

private:
    id_t id_{0};
    std::string name_;

    FieldMatrix field_;
    std::map<id_t, std::shared_ptr<Entity>> entities_;
};

} // namespace game

#endif //INC_3_LEVEL_H