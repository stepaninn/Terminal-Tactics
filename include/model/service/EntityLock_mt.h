#ifndef MYGAMEPROJECT_ENTITYLOCK_MT_H
#define MYGAMEPROJECT_ENTITYLOCK_MT_H

#include "types_mt.h"

#include <algorithm>
#include <memory>
#include <mutex>
#include <tbb/concurrent_hash_map.h>

namespace game::mt::service {

struct EntityLocks {
    std::unique_lock<std::mutex> first;
    std::unique_lock<std::mutex> second;
};

class EntityLockPool {
public:
    static EntityLockPool& instance() {
        static EntityLockPool pool;
        return pool;
    }

    [[nodiscard]] EntityLocks lock_entities(game::mt::EntityId a, game::mt::EntityId b) {
        if (a == b) {
            return {lock_entity(a), std::unique_lock<std::mutex>()};
        }
        auto first_id = std::min(a, b);
        auto second_id = std::max(a, b);
        auto first_lock = lock_entity(first_id);
        auto second_lock = lock_entity(second_id);
        return {std::move(first_lock), std::move(second_lock)};
    }

    [[nodiscard]] std::unique_lock<std::mutex> lock_entity(game::mt::EntityId id) {
        return std::unique_lock<std::mutex>(*get_mutex(id));
    }

private:
    using LockMap = tbb::concurrent_hash_map<game::mt::EntityId, std::shared_ptr<std::mutex>>;

    std::shared_ptr<std::mutex> get_mutex(game::mt::EntityId id) {
        LockMap::accessor acc;
        if (!locks_.insert(acc, id)) {
            return acc->second;
        }
        acc->second = std::make_shared<std::mutex>();
        return acc->second;
    }

    LockMap locks_;
};

}

#endif // MYGAMEPROJECT_ENTITYLOCK_MT_H
