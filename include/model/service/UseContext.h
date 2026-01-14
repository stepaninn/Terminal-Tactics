#ifndef MYGAMEPROJECT_USECONTEXT_H
#define MYGAMEPROJECT_USECONTEXT_H

#include "types.h"
#include "model/entity/components/InventoryComponent.h"

namespace game::entity::components {
class HealthComponent;
class TimePointsComponent;
class WeaponComponent;
}

namespace game::service {

/// @brief Стурктура контекст для использования предметов
struct UseContext {
    game::EntityId user{};
    game::EntityId target{};

    game::entity::components::HealthComponent* hp = nullptr;
    game::entity::components::TimePointsComponent* tp = nullptr;
    game::entity::components::WeaponComponent* wp = nullptr;
    game::entity::components::InventoryComponent* inv = nullptr;
};

}

#endif //MYGAMEPROJECT_USECONTEXT_H
