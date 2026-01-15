#ifndef MYGAMEPROJECT_NCURSESVIEW_H
#define MYGAMEPROJECT_NCURSESVIEW_H

#include "view/IView.h"
#include "model/service/events/EventBus.h"

#include <ncurses.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace game::view {

class NcursesView final : public IView {
public:
    explicit NcursesView(std::shared_ptr<game::service::events::EventBus> bus = nullptr);
    ~NcursesView() override;

    void render(const game::service::World& world,
                const game::controller::Controller& controller) override;
    game::controller::InputAction poll_input() override;

private:
    void draw_map(const game::service::World& world,
                  const game::controller::Controller& controller);
    void draw_hud(const game::service::World& world,
                  const game::controller::Controller& controller);
    std::string enter_path(const char* label);

    std::shared_ptr<game::service::events::EventBus> bus_;
    std::string last_message_{"Ready"};
    game::EntityId moving_entity_{0};
    std::optional<game::Position> anim_pos_;
    std::vector<game::Position> anim_path_;
    bool animating_{false};
};

}

#endif // MYGAMEPROJECT_NCURSESVIEW_H
