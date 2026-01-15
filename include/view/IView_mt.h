#ifndef MYGAMEPROJECT_IVIEW_MT_H
#define MYGAMEPROJECT_IVIEW_MT_H

#include "controller/Controller_mt.h"
#include "model/service/World_mt.h"

namespace game::mt::view {

class IView {
public:
    virtual ~IView() = default;

    virtual void render(const game::mt::service::World& world,
                        const game::mt::controller::Controller& controller) = 0;
    virtual game::mt::controller::InputAction poll_input() = 0;
};

}

#endif // MYGAMEPROJECT_IVIEW_MT_H
