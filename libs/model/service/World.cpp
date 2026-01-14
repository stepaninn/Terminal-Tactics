#include "model/service/World.h"

namespace game::service {

VisibilityMap* World::unit_visibility(game::EntityId entity_id) {
    if (!level_) return nullptr;
    auto it = unit_fov_.find(entity_id);
    if (it != unit_fov_.end()) return it->second.get();
    auto map = std::make_unique<VisibilityMap>(level_->get_width(), level_->get_height());
    unit_fov_[entity_id] = std::move(map);
    return unit_fov_[entity_id].get();
}

VisibilityMap* World::team_visibility(game::TeamId team_id) {
    if (!level_) return nullptr;
    auto it = team_visible_.find(team_id);
    if (it != team_visible_.end()) return it->second.get();
    auto map = std::make_unique<VisibilityMap>(level_->get_width(), level_->get_height());
    team_visible_[team_id] = std::move(map);
    return team_visible_[team_id].get();
}

VisibilityMap* World::team_exploration(game::TeamId team_id) {
    if (!level_) return nullptr;
    auto it = team_explored_.find(team_id);
    if (it != team_explored_.end()) return it->second.get();
    auto map = std::make_unique<VisibilityMap>(level_->get_width(), level_->get_height());
    team_explored_[team_id] = std::move(map);
    return team_explored_[team_id].get();
}

void World::make_teams() {
    if (!level_) return;
    auto ent = level_->get_entities();
    for (const auto* e : ent) {
        teams_[e->get_team_id()].insert(e->get_id());
    }
}

void World::remove_from_team(game::TeamId team_id, game::EntityId id) {
    if (auto it = teams_.find(team_id); it != teams_.end()) {
        auto& list = it->second;
        list.erase(id);
    }
}

void World::resize(size_t rows, size_t cols) {
    if (!level_) return;
    level_->resize_field(rows, cols);
    for (auto& map : unit_fov_ | std::views::values) map->resize(rows, cols);
    for (auto& map : team_visible_ | std::views::values) map->resize(rows, cols);
    for (auto& map : team_explored_ | std::views::values) map->resize(rows, cols);
}

game::entity::Entity* World::spawn_entity(std::unique_ptr<game::entity::Entity> e, game::Position pos) {
    if (!level_) return nullptr;
    auto* res = level_->spawn_entity(std::move(e), pos);
    if (!res) return nullptr;
    teams_[res->get_team_id()].insert(res->get_id());
    return res;
}

std::unique_ptr<game::entity::Entity> World::remove_entity(const game::entity::Entity* e) {
    if (!level_ || !e) return nullptr;
    remove_from_team(e->get_team_id(), e->get_id());
    return level_->remove_entity(e);
}

std::unique_ptr<game::entity::Entity> World::remove_entity(game::EntityId id) {
    if (!level_) return nullptr;
    if (auto* e = level_->get_entity(id)) {
        remove_from_team(e->get_team_id(), e->get_id());
    }
    return level_->remove_entity(id);
}

bool World::set_entity_team(game::EntityId id, game::TeamId team_id) {
    if (!level_) return false;
    auto* e = level_->get_entity(id);
    if (!e) return false;
    if (e->get_team_id() == team_id) return true;
    remove_from_team(e->get_team_id(), id);
    e->set_team_id(team_id);
    teams_[team_id].insert(id);
    return true;
}

}
