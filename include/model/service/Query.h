#ifndef INC_3_TMP_SERVICE_QUERY_H
#define INC_3_TMP_SERVICE_QUERY_H

#include "model/entity/entities/Entity.h"

namespace game::service {

template<typename... Comps>
[[nodiscard]] bool has_components(const game::entity::Entity& entity) {
    return (entity.get_component<Comps>() && ...);
}

}

#endif // INC_3_TMP_SERVICE_QUERY_H
