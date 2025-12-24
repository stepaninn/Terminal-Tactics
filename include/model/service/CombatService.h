#ifndef INC_3_TMP_SERVICE_COMBATSERVICE_H
#define INC_3_TMP_SERVICE_COMBATSERVICE_H

#include "model/repository/Level.h"
#include "ServiceBase.h"

#include <memory>

namespace game::service {

class CombatService : public ServiceBase {
public:
    explicit CombatService(std::shared_ptr<events::EventBus> bus = nullptr) : ServiceBase(std::move(bus)) {}

    [[nodiscard]] bool apply_damage(game::repo::Level& level, game::EntityId attacker_id,
                                    game::EntityId target_id, int amount);
};

}

#endif // INC_3_TMP_SERVICE_COMBATSERVICE_H
