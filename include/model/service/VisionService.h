#ifndef MYGAMEPROJECT_VISIONSERVICE_H
#define MYGAMEPROJECT_VISIONSERVICE_H

#include "ServiceBase.h"
#include "model/entity/components/VisionComponent.h"
#include "model/service/World.h"

namespace game::repo {
class Level;
}

namespace game::service {

class VisionService : public ServiceBase {
public:
    explicit VisionService(std::shared_ptr<events::EventBus> bus = nullptr)
        : ServiceBase(std::move(bus)) {}

    // update_unit_fov(World&, EntityId) пересчет FOV юнита
    // rebuild_team_visible(World&, TeamId) OR всех юнитов команды
    // update_team_explored(World&, TeamId) либо встроить в rebuild_team_visible
    // rebuild_all_fov(World&) для полной пересборки

    [[nodiscard]] bool has_line_of_fire(const game::repo::Level& lvl, game::Position from, game::Position to);

    [[nodiscard]] std::vector<EntityId> visible_entities(game::service::World& w, game::EntityId observer_id, int radius);
private:
    static void cast_light(VisibilityMap& map, const game::repo::Level& lvl, int x, int y, int radius, int row,
        double start_slope, double end_slope, int xx, int xy, int yx, int yy);

    [[nodiscard]] static VisibilityMap compute_fov(const game::repo::Level& lvl, Position pos, int r);
};

}

#endif //MYGAMEPROJECT_VISIONSERVICE_H
