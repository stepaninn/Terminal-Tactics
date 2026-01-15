#ifndef MYGAMEPROJECT_LOADSAVESERVICE_H
#define MYGAMEPROJECT_LOADSAVESERVICE_H

#include "ServiceBase.h"
#include "model/service/World.h"

#include <iosfwd>
#include <memory>
#include <string>

namespace game::entity::components { enum class AIBehavior; }
namespace game::entity::items { class Item; }
namespace game::repo::cells { class ICell;}

namespace game::service {

class LoadSaveService : public ServiceBase {
public:
    explicit LoadSaveService(std::shared_ptr<events::EventBus> bus = nullptr)
        : ServiceBase(std::move(bus)) {}

    [[nodiscard]] bool save(const World& world, const std::string& path) const;
    [[nodiscard]] std::unique_ptr<World> load(const std::string& path) const;

private:
    static std::string ammo_type_string(game::AmmoType type);
    static bool parse_ammo_type(const std::string& token, game::AmmoType& out);

    static std::string behavior_string(::game::entity::components::AIBehavior behavior);
    static bool parse_behavior(const std::string& token, ::game::entity::components::AIBehavior& out);

    static std::string cell_type_string(const ::game::repo::cells::ICell& cell);
    static std::unique_ptr<::game::repo::cells::ICell> make_cell(const std::string& type, bool broken);

    static void save_item(std::ostream& out, const ::game::entity::items::Item& item);
    static std::unique_ptr<::game::entity::items::Item> load_item(std::istream& in);
};

}

#endif // MYGAMEPROJECT_LOADSAVESERVICE_H
