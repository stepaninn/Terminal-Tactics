#ifndef MYGAMEPROJECT_IVIEW_H
#define MYGAMEPROJECT_IVIEW_H

#include "controller/Controller.h"
#include "model/service/World.h"

namespace game::view {

class IView {
public:
    virtual ~IView() = default;

    virtual void render(const game::service::World& world,
                        const game::controller::Controller& controller) = 0;
    virtual game::controller::InputAction poll_input() = 0;
};

}

#endif // MYGAMEPROJECT_IVIEW_H
