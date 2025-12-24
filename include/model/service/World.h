#ifndef INC_3_TMP_SERVICE_WORLD_H
#define INC_3_TMP_SERVICE_WORLD_H

#include "../repository/Level.h"
#include "events/EventBus.h"
#include "Query.h"

#include <memory>
#include <vector>

namespace game::service {

class World {
public:
    explicit World(std::shared_ptr<game::repo::Level> level, std::shared_ptr<events::EventBus> bus = nullptr)
        : level_(std::move(level)), event_bus_(std::move(bus)) {}

    [[nodiscard]] std::shared_ptr<game::repo::Level> level() const noexcept { return level_; }
    [[nodiscard]] events::EventBus* events() const noexcept { return event_bus_.get(); }

    [[nodiscard]] game::entity::Entity* entity(game::EntityId id) const {
        return level_ ? level_->get_entity(id) : nullptr;
    }

    [[nodiscard]] std::vector<const game::entity::Entity*> entities() const {
        if (!level_) return {};
        return level_->get_entities();
    }

    template<typename... Comps>
    [[nodiscard]] std::vector<const game::entity::Entity*> view() const {
        std::vector<const game::entity::Entity*> res;
        if (!level_) return res;
        auto all = level_->get_entities();
        res.reserve(all.size());
        for (const auto* entity : all) {
            if (entity && has_components<Comps...>(*entity)) res.push_back(entity);
        }
        return res;
    }

private:
    std::shared_ptr<game::repo::Level> level_;
    std::shared_ptr<events::EventBus> event_bus_;
};

}

#endif // INC_3_TMP_SERVICE_WORLD_H
