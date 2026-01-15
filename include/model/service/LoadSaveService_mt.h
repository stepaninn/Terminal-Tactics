#ifndef MYGAMEPROJECT_LOADSAVESERVICE_MT_H
#define MYGAMEPROJECT_LOADSAVESERVICE_MT_H

#include "ServiceBase_mt.h"
#include "model/service/World_mt.h"

#include <iosfwd>
#include <mutex>
#include <memory>
#include <string>

namespace game::mt::entity::components { enum class AIBehavior; }
namespace game::mt::entity::items { class Item; }
namespace game::mt::repo::cells { class ICell;}

namespace game::mt::service {

class LoadSaveService : public ServiceBase {
public:
    explicit LoadSaveService(std::shared_ptr<events::EventBus> bus = nullptr)
        : ServiceBase(std::move(bus)) {}

    [[nodiscard]] bool save(const World& world, const std::string& path) const;
    [[nodiscard]] std::unique_ptr<World> load(const std::string& path) const;

private:
    static std::string ammo_type_string(game::mt::AmmoType type);
    static bool parse_ammo_type(const std::string& token, game::mt::AmmoType& out);

    static std::string behavior_string(::game::mt::entity::components::AIBehavior behavior);
    static bool parse_behavior(const std::string& token, ::game::mt::entity::components::AIBehavior& out);

    static std::string cell_type_string(const ::game::mt::repo::cells::ICell& cell);
    static std::unique_ptr<::game::mt::repo::cells::ICell> make_cell(const std::string& type, bool broken);

    static void save_item(std::ostream& out, const ::game::mt::entity::items::Item& item);
    static std::unique_ptr<::game::mt::entity::items::Item> load_item(std::istream& in);

    mutable std::mutex mutex_;
};

}

#endif // MYGAMEPROJECT_LOADSAVESERVICE_MT_H
