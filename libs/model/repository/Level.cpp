#include "model/repository/Level.h"

#include <utility>

namespace game::repo {

std::vector<const game::entity::Entity*> Level::get_entities() noexcept {
    std::vector<const game::entity::Entity*> res;
    res.reserve(entities_.size());
    for (const auto& entity : entities_ | std::views::values) {
        res.push_back(entity.get());
    }
    return res;
}

cells::ICell* Level::get_cell(game::Position pos) const noexcept {
    if (pos.x >= field_.rows() || pos.y >= field_.cols()) return nullptr;
    return field_(pos.x, pos.y).get();
}

std::unique_ptr<cells::ICell> Level::set_cell(game::Position pos, std::unique_ptr<cells::ICell> cell) {
    if (pos.x >= field_.rows() || pos.y >= field_.cols()) return cell;
    return std::exchange(field_(pos.x, pos.y), std::move(cell));
}

void Level::spawn_entity(std::unique_ptr<game::entity::Entity> e, game::Position pos) {
    if (!e) return;
    e->set_id(next_entity_id_++);
    game::EntityId id = e->get_id();
    entities_[id] = std::move(e);
    entity_positions_[id] = pos;
}

bool Level::move_entity(game::EntityId id, game::Position to) {
    if (to.x >= field_.rows() || to.y >= field_.cols()) return false;
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
