#ifndef MYGAMEPROJECT_USECONTEXT_MT_H
#define MYGAMEPROJECT_USECONTEXT_MT_H

#include "types_mt.h"
#include "model/entity/components/InventoryComponent_mt.h"

namespace game::mt::entity::components {
class HealthComponent;
class TimePointsComponent;
class WeaponComponent;
}

namespace game::mt::service {

/// @brief Стурктура контекст для использования предметов
struct UseContext {
    game::mt::EntityId user{};
    game::mt::EntityId target{};

    game::mt::entity::components::HealthComponent* hp = nullptr;
    game::mt::entity::components::TimePointsComponent* tp = nullptr;
    game::mt::entity::components::WeaponComponent* wp = nullptr;
    game::mt::entity::components::InventoryComponent* inv = nullptr;
};

}

#endif //MYGAMEPROJECT_USECONTEXT_MT_H
