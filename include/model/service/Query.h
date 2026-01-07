#ifndef MYGAMEPROJECT_QUERY_H
#define MYGAMEPROJECT_QUERY_H

#include "model/entity/entities/Entity.h"

namespace game::service {

template<typename... Comps>
[[nodiscard]] bool has_components(const game::entity::Entity& entity) {
    return (entity.get_component<Comps>() && ...);
}

}

#endif // MYGAMEPROJECT_QUERY_H
