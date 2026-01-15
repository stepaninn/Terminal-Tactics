#include "model/service/World_mt.h"

#include <shared_mutex>

namespace game::mt::service {

VisibilityMap* World::unit_visibility(game::mt::EntityId entity_id) {
    if (!level_) return nullptr;
    std::unique_lock<std::shared_mutex> lock(fov_mutex_);
    tbb::concurrent_hash_map<game::mt::EntityId, std::unique_ptr<VisibilityMap>>::accessor acc;
    if (unit_fov_.find(acc, entity_id)) return acc->second.get();
    auto map = std::make_unique<VisibilityMap>(level_->get_width(), level_->get_height());
    unit_fov_.insert(acc, entity_id);
    acc->second = std::move(map);
    return acc->second.get();
}

VisibilityMap* World::team_visibility(game::mt::TeamId team_id) {
    if (!level_) return nullptr;
    std::unique_lock<std::shared_mutex> lock(fov_mutex_);
    tbb::concurrent_hash_map<game::mt::TeamId, std::unique_ptr<VisibilityMap>>::accessor acc;
    if (team_visible_.find(acc, team_id)) return acc->second.get();
    auto map = std::make_unique<VisibilityMap>(level_->get_width(), level_->get_height());
    team_visible_.insert(acc, team_id);
    acc->second = std::move(map);
    return acc->second.get();
}

VisibilityMap* World::team_exploration(game::mt::TeamId team_id) {
    if (!level_) return nullptr;
    std::unique_lock<std::shared_mutex> lock(fov_mutex_);
    tbb::concurrent_hash_map<game::mt::TeamId, std::unique_ptr<VisibilityMap>>::accessor acc;
    if (team_explored_.find(acc, team_id)) return acc->second.get();
    auto map = std::make_unique<VisibilityMap>(level_->get_width(), level_->get_height());
    team_explored_.insert(acc, team_id);
    acc->second = std::move(map);
    return acc->second.get();
}

void World::make_teams() {
    if (!level_) return;
    auto ent = level_->get_entities();
    std::unique_lock<std::shared_mutex> lock(teams_mutex_);
    for (const auto& e : ent) {
        tbb::concurrent_hash_map<game::mt::TeamId, std::set<game::mt::EntityId>>::accessor acc;
        if (teams_.insert(acc, e->get_team_id())) {
            acc->second = {};
        }
        acc->second.insert(e->get_id());
    }
}

void World::remove_from_team(game::mt::TeamId team_id, game::mt::EntityId id) {
    std::unique_lock<std::shared_mutex> lock(teams_mutex_);
    tbb::concurrent_hash_map<game::mt::TeamId, std::set<game::mt::EntityId>>::accessor acc;
    if (teams_.find(acc, team_id)) {
        acc->second.erase(id);
    }
}

void World::resize(size_t rows, size_t cols) {
    if (!level_) return;
    level_->resize_field(rows, cols);
    std::unique_lock<std::shared_mutex> lock(fov_mutex_);
    for (auto& [_, map] : unit_fov_) map->resize(rows, cols);
    for (auto& [_, map] : team_visible_) map->resize(rows, cols);
    for (auto& [_, map] : team_explored_) map->resize(rows, cols);
}

std::shared_ptr<game::mt::entity::Entity> World::spawn_entity(std::unique_ptr<game::mt::entity::Entity> e,
                                                              game::mt::Position pos) {
    if (!level_) return nullptr;
    auto res = level_->spawn_entity(std::move(e), pos);
    if (!res) return nullptr;
    std::unique_lock<std::shared_mutex> lock(teams_mutex_);
    tbb::concurrent_hash_map<game::mt::TeamId, std::set<game::mt::EntityId>>::accessor acc;
    if (teams_.insert(acc, res->get_team_id())) {
        acc->second = {};
    }
    acc->second.insert(res->get_id());
    return res;
}

void World::set_level(std::unique_ptr<game::mt::repo::Level> level) {
    level_ = std::move(level);
    {
        std::unique_lock<std::shared_mutex> lock(fov_mutex_);
        unit_fov_.clear();
        team_visible_.clear();
        team_explored_.clear();
    }
    {
        std::unique_lock<std::shared_mutex> lock(teams_mutex_);
        teams_.clear();
    }
    make_teams();
}

std::unique_ptr<game::mt::repo::Level> World::take_level() {
    {
        std::unique_lock<std::shared_mutex> lock(fov_mutex_);
        unit_fov_.clear();
        team_visible_.clear();
        team_explored_.clear();
    }
    {
        std::unique_lock<std::shared_mutex> lock(teams_mutex_);
        teams_.clear();
    }
    return std::move(level_);
}

std::shared_ptr<game::mt::entity::Entity> World::remove_entity(const game::mt::entity::Entity* e) {
    if (!level_ || !e) return nullptr;
    remove_from_team(e->get_team_id(), e->get_id());
    return level_->remove_entity(e);
}

std::shared_ptr<game::mt::entity::Entity> World::remove_entity(game::mt::EntityId id) {
    if (!level_) return nullptr;
    if (auto e = level_->get_entity(id)) {
        remove_from_team(e->get_team_id(), e->get_id());
    }
    return level_->remove_entity(id);
}

bool World::set_entity_team(game::mt::EntityId id, game::mt::TeamId team_id) {
    if (!level_) return false;
    auto e = level_->get_entity(id);
    if (!e) return false;
    if (e->get_team_id() == team_id) return true;
    remove_from_team(e->get_team_id(), id);
    e->set_team_id(team_id);
    std::unique_lock<std::shared_mutex> lock(teams_mutex_);
    tbb::concurrent_hash_map<game::mt::TeamId, std::set<game::mt::EntityId>>::accessor acc;
    if (teams_.insert(acc, team_id)) {
        acc->second = {};
    }
    acc->second.insert(id);
    return true;
}

}
