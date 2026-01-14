#ifndef MYGAMEPROJECT_AIBEHAVIOR_H
#define MYGAMEPROJECT_AIBEHAVIOR_H

#include "World.h"
#include "MovementService.h"
#include "CombatService.h"
#include "ItemService.h"
#include "VisionService.h"

namespace game::service {

struct AIContext {
    MovementService& move;
    CombatService& combat;
    ItemService& items;
    VisionService& vision;
};

/// @brief Интерфейс поведения ИИ
class IBehavior {
public:
    virtual ~IBehavior() = default;
    virtual void act(World& w, EntityId id, AIContext& ctx) = 0;
};

}

#endif // MYGAMEPROJECT_AIBEHAVIOR_H
