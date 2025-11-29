#ifndef INC_3_GAMESYSTEM_H
#define INC_3_GAMESYSTEM_H

#include <memory>
#include "types.h"
#include "ISystem.h"
#include "SystemManager.h"

namespace game {

class GameSystem : public ISystem {
public:
    GameSystem() = default;
    explicit GameSystem(std::shared_ptr<SystemManager> mgr);

    EventBus& get_event_bus() override;

    std::shared_ptr<Level> get_level() const noexcept { return game_; }
    void set_level(std::shared_ptr<Level> lvl) { game_ = std::move(lvl); }

    virtual void start() {}
    virtual void pause() {}

    void update(std::shared_ptr<Level> lvl) override;

protected:
    std::shared_ptr<Level> game_;
    bool player_turn_ = false;
    std::weak_ptr<SystemManager> sys_mgr_;
};

} // namespace game


#endif //INC_3_GAMESYSTEM_H