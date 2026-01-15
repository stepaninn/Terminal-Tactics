#ifndef MYGAMEPROJECT_NCURSESVIEW_MT_H
#define MYGAMEPROJECT_NCURSESVIEW_MT_H

#include "view/IView_mt.h"
#include "model/service/events/EventBus_mt.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace game::mt::view {

class NcursesView final : public IView {
public:
    explicit NcursesView(std::shared_ptr<game::mt::service::events::EventBus> bus = nullptr);
    ~NcursesView() override;

    void render(const game::mt::service::World& world,
                const game::mt::controller::Controller& controller) override;
    game::mt::controller::InputAction poll_input() override;

private:
    void draw_map(const game::mt::service::World& world,
                  const game::mt::controller::Controller& controller);
    void draw_hud(const game::mt::service::World& world,
                  const game::mt::controller::Controller& controller);
    std::string enter_path(const char* label);

    std::shared_ptr<game::mt::service::events::EventBus> bus_;
    std::string last_message_{"Ready"};
    game::mt::EntityId moving_entity_{0};
    std::optional<game::mt::Position> anim_pos_;
    std::vector<game::mt::Position> anim_path_;
    bool animating_{false};
};

}

#endif // MYGAMEPROJECT_NCURSESVIEW_MT_H
