#ifndef MYGAMEPROJECT_AIBEHAVIOR_MT_H
#define MYGAMEPROJECT_AIBEHAVIOR_MT_H

#include "World_mt.h"
#include "MovementService_mt.h"
#include "CombatService_mt.h"
#include "ItemService_mt.h"
#include "VisionService_mt.h"

namespace game::mt::service {

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

#endif // MYGAMEPROJECT_AIBEHAVIOR_MT_H
