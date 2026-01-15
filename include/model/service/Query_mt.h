#ifndef MYGAMEPROJECT_QUERY_MT_H
#define MYGAMEPROJECT_QUERY_MT_H

#include "model/entity/entities/Entity_mt.h"

namespace game::mt::service {

template<typename... Comps>
[[nodiscard]] bool has_components(const game::mt::entity::Entity& entity) {
    return (entity.get_component<Comps>() && ...);
}

}

#endif // MYGAMEPROJECT_QUERY_MT_H
