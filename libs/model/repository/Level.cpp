#include "model/repository/Level.h"

#include <utility>

namespace game::repo {

std::vector<const game::entity::Entity*> Level::get_entities() const noexcept {
    std::vector<const game::entity::Entity*> res;
    res.reserve(entities_.size());
    for (const auto& entity : entities_ | std::views::values) {
        res.push_back(entity.get());
    }
    return res;
}

cells::ICell* Level::get_cell(game::Position pos) const noexcept {
    if (!in_bounds(pos)) return nullptr;
    return field_(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)).get();
}

std::unique_ptr<cells::ICell> Level::set_cell(game::Position pos, std::unique_ptr<cells::ICell> cell) {
    if (!in_bounds(pos)) return cell;
    return std::exchange(field_(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)), std::move(cell));
}

game::entity::Entity* Level::spawn_entity(std::unique_ptr<game::entity::Entity> e, game::Position pos) {
    if (!e) return nullptr;
    game::EntityId id = e->get_id();
    if (entities_.contains(id)) return nullptr;
    entities_[id] = std::move(e);
    entity_positions_[id] = pos;
    return entities_[id].get();
}

bool Level::move_entity(game::EntityId id, game::Position to) {
    if (!in_bounds(to)) return false;
    auto ent_it = entities_.find(id);
    if (ent_it == entities_.end()) return false;
    entity_positions_[id] = to;
    return true;
}

std::unique_ptr<game::entity::Entity> Level::remove_entity(const game::entity::Entity* e) {
    if (!e) return nullptr;
    return remove_entity(e->get_id());
}

std::unique_ptr<game::entity::Entity> Level::remove_entity(game::EntityId id) {
    auto it = entities_.find(id);
    if (it == entities_.end()) return nullptr;
    auto res = std::move(it->second);
    entities_.erase(it);
    entity_positions_.erase(id);
    return res;
}

const game::Position* Level::get_entity_position(game::EntityId id) const noexcept {
    auto it = entity_positions_.find(id);
    if (it == entity_positions_.end()) return nullptr;
    return &it->second;
}

std::vector<const game::entity::Entity*> Level::get_entities_radius(game::Position pos, int r) const {
    std::vector<const game::entity::Entity*> res;
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

}
