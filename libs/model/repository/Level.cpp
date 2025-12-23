#include "model/repository/Level.h"

#include <utility>

namespace game {

std::vector<const Entity*> Level::get_entities() noexcept {
    std::vector<const Entity*> res;
    res.reserve(entities_.size());
    for (const auto& entity : entities_ | std::views::values) {
        res.push_back(entity.get());
    }
    return res;
}

ICell* Level::get_cell(Position pos) const noexcept {
    if (pos.x >= field_.rows() || pos.y >= field_.cols()) return nullptr;
    return field_(pos.x, pos.y).get();
}

std::unique_ptr<ICell> Level::set_cell(Position pos, std::unique_ptr<ICell> cell) {
    if (pos.x >= field_.rows() || pos.y >= field_.cols()) return nullptr;
    return std::exchange(field_(pos.x, pos.y), std::move(cell));
}

void Level::spawn_entity(std::unique_ptr<Entity> e, Position pos) {
    if (!e) return;
    e->set_id(next_entity_id_++);
    EntityId id = e->get_id();
    entities_[id] = std::move(e);
    entity_positions_[id] = pos;
}

std::unique_ptr<Entity> Level::remove_entity(const Entity* e) {
    if (!e) return nullptr;
    return remove_entity(e->get_id());
}

std::unique_ptr<Entity> Level::remove_entity(EntityId id) {
    auto it = entities_.find(id);
    if (it == entities_.end()) return nullptr;
    auto res = std::move(it->second);
    entities_.erase(it);
    entity_positions_.erase(id);
    return res;
}

std::vector<const Entity*> Level::get_entities_radius(Position pos, int r) const {
    std::vector<const Entity*> res;
    if (r < 0) return res;
    long long rr = static_cast<long long>(r) * r;
    for (const auto& [id, entity_pos] : entity_positions_) {
        long long dx = static_cast<long long>(entity_pos.x) - static_cast<long long>(pos.x);
        long long dy = static_cast<long long>(entity_pos.y) - static_cast<long long>(pos.y);
        if (dx * dx + dy * dy <= rr) {
            auto it = entities_.find(id);
            if (it != entities_.end()) res.push_back(it->second.get());
        }
    }
    return res;
}

} // namespace game
