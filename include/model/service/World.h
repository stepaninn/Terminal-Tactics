#ifndef MYGAMEPROJECT_WORLD_H
#define MYGAMEPROJECT_WORLD_H

#include "model/repository/Level.h"
#include "events/EventBus.h"
#include "Query.h"

#include <memory>
#include <vector>

namespace game::service {

class World {
public:
    explicit World(std::unique_ptr<game::repo::Level> level, std::unique_ptr<events::EventBus> bus = nullptr)
        : level_(std::move(level)), event_bus_(std::move(bus)) {}

    /**
     * @brief Метод, возвращающий текущий уровень
     * @return Текущий уровень
     */
    [[nodiscard]] game::repo::Level* level() const noexcept { return level_.get(); }
    /**
     * @brief Метод, возвращающий текущий EventBus
     * @return Текущий EventBus
     */
    [[nodiscard]] events::EventBus* events() const noexcept { return event_bus_.get(); }

    /**
     * @brief Метод поиска существа по ID на текущем уровне
     * @param id ID искомого существа
     * @return Указатель на искомое существо
     */
    [[nodiscard]] game::entity::Entity* entity(game::EntityId id) const {
        return level_ ? level_->get_entity(id) : nullptr;
    }

    /**
     * @brief Метод получения существ на текущем уровне
     * @return Массив существ на текущем уровне
     */
    [[nodiscard]] std::vector<const game::entity::Entity*> entities() const {
        if (!level_) return {};
        return level_->get_entities();
    }

    /**
     * @brief Метод, возвращаюший существ по компоненту
     * @tparam Comps Типы компонентов, по которым осуществляется поиск
     * @return Массив существ, которые имеют данный компонент
     */
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
    std::unique_ptr<game::repo::Level> level_;
    std::unique_ptr<events::EventBus> event_bus_;
};

}

#endif // MYGAMEPROJECT_WORLD_H
