#include "model/repository/Level_mt.h"
#include "model/repository/cells/DestructibleCell_mt.h"
#include "model/repository/cells/Floor_mt.h"

#include <shared_mutex>
#include <utility>

namespace game::mt::repo {

std::vector<std::shared_ptr<const game::mt::entity::Entity>> Level::get_entities() const noexcept {
    std::vector<std::shared_ptr<const game::mt::entity::Entity>> res;
    std::shared_lock<std::shared_mutex> lock(entities_mutex_);
    res.reserve(entities_.size());
    for (const auto& entry : entities_) {
        res.push_back(entry.second);
    }
    return res;
}

cells::ICell* Level::get_cell(game::mt::Position pos) const noexcept {
    if (!in_bounds(pos)) return nullptr;
    std::shared_lock<std::shared_mutex> lock(field_mutex_);
    return field_(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)).get();
}

std::shared_ptr<game::mt::entity::items::Item> Level::add(game::mt::Position pos,
                                                      std::shared_ptr<game::mt::entity::items::Item> item) const {
    if (!item) return item;
    auto* cell = get_cell(pos);
    if (!cell) return item;
    auto* container = dynamic_cast<cells::IItemContainer*>(cell);
    if (!container) return item;
    return container->add(std::move(item));
}

std::unique_ptr<cells::ICell> Level::set_cell(game::mt::Position pos, std::unique_ptr<cells::ICell> cell) {
    if (!in_bounds(pos)) return cell;
    std::unique_lock<std::shared_mutex> lock(field_mutex_);
    return std::exchange(field_(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)), std::move(cell));
}

std::shared_ptr<game::mt::entity::Entity> Level::spawn_entity(std::unique_ptr<game::mt::entity::Entity> e,
                                                              game::mt::Position pos) {
    if (!e) return nullptr;
    auto entity = std::shared_ptr<game::mt::entity::Entity>(std::move(e));
    game::mt::EntityId id = entity->get_id();
    std::unique_lock<std::shared_mutex> lock(entities_mutex_);
    tbb::concurrent_hash_map<game::mt::EntityId, std::shared_ptr<game::mt::entity::Entity>>::accessor acc;
    if (!entities_.insert(acc, id)) return nullptr;
    acc->second = std::move(entity);
    tbb::concurrent_hash_map<game::mt::EntityId, game::mt::Position>::accessor pos_acc;
    entity_positions_.insert(pos_acc, id);
    pos_acc->second = pos;
    return acc->second;
}

bool Level::move_entity(game::mt::EntityId id, game::mt::Position to) {
    if (!in_bounds(to)) return false;
    std::unique_lock<std::shared_mutex> lock(entities_mutex_);
    tbb::concurrent_hash_map<game::mt::EntityId, std::shared_ptr<game::mt::entity::Entity>>::const_accessor acc;
    if (!entities_.find(acc, id)) return false;
    tbb::concurrent_hash_map<game::mt::EntityId, game::mt::Position>::accessor pos_acc;
    entity_positions_.insert(pos_acc, id);
    pos_acc->second = to;
    return true;
}

std::shared_ptr<game::mt::entity::Entity> Level::remove_entity(const game::mt::entity::Entity* e) {
    if (!e) return nullptr;
    return remove_entity(e->get_id());
}

std::shared_ptr<game::mt::entity::Entity> Level::remove_entity(game::mt::EntityId id) {
    std::unique_lock<std::shared_mutex> lock(entities_mutex_);
    tbb::concurrent_hash_map<game::mt::EntityId, std::shared_ptr<game::mt::entity::Entity>>::accessor acc;
    if (!entities_.find(acc, id)) return nullptr;
    auto res = std::move(acc->second);
    entities_.erase(acc);
    tbb::concurrent_hash_map<game::mt::EntityId, game::mt::Position>::accessor pos_acc;
    if (entity_positions_.find(pos_acc, id)) {
        entity_positions_.erase(pos_acc);
    }
    return res;
}

std::optional<game::mt::Position> Level::get_entity_position(game::mt::EntityId id) const noexcept {
    std::shared_lock<std::shared_mutex> lock(entities_mutex_);
    tbb::concurrent_hash_map<game::mt::EntityId, game::mt::Position>::const_accessor acc;
    if (!entity_positions_.find(acc, id)) return std::nullopt;
    return acc->second;
}

std::shared_ptr<game::mt::entity::Entity> Level::get_entity_at(Position pos) {
    std::shared_lock<std::shared_mutex> lock(entities_mutex_);
    for (const auto& ent : entity_positions_) {
        if (ent.second != pos) continue;

        tbb::concurrent_hash_map<game::mt::EntityId,
                                 std::shared_ptr<game::mt::entity::Entity>>::const_accessor acc;
        if (entities_.find(acc, ent.first)) return acc->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<const game::mt::entity::Entity>> Level::get_entities_radius(game::mt::Position pos,
                                                                                        int r) const {
    std::vector<std::shared_ptr<const game::mt::entity::Entity>> res;
    if (r < 0) return res;
    long long rr = static_cast<long long>(r) * r;
    std::shared_lock<std::shared_mutex> lock(entities_mutex_);
    for (const auto& [id, entity_pos] : entity_positions_) {
        long long dx = static_cast<long long>(entity_pos.x) - static_cast<long long>(pos.x);
        long long dy = static_cast<long long>(entity_pos.y) - static_cast<long long>(pos.y);
        if (dx * dx + dy * dy <= rr) {
            tbb::concurrent_hash_map<game::mt::EntityId, std::shared_ptr<game::mt::entity::Entity>>::const_accessor acc;
            if (entities_.find(acc, id)) res.push_back(acc->second);
        }
    }
    return res;
}

bool Level::try_shoot(int x, int y) noexcept {
    if (!in_bounds(x, y)) return false;
    std::unique_lock<std::shared_mutex> lock(field_mutex_);
    auto& cell_slot = field_(static_cast<size_t>(x), static_cast<size_t>(y));
    auto* cell = cell_slot.get();
    auto* destructible = dynamic_cast<cells::IDestructibleCell*>(cell);
    if (!destructible || !destructible->can_be_shot()) return false;
    bool changed = destructible->apply_shot();
    if (changed) {
        cell_slot = std::make_unique<cells::Floor>();
    }
    return changed;
}

}
