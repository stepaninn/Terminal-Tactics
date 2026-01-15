#include "model/service/LoadSaveService.h"

#include "model/entity/components/AIComponent.h"
#include "model/entity/components/CombatComponent.h"
#include "model/entity/components/HealthComponent.h"
#include "model/entity/components/InventoryComponent.h"
#include "model/entity/components/MeleeComponent.h"
#include "model/entity/components/MoveComponent.h"
#include "model/entity/components/TimePointsComponent.h"
#include "model/entity/components/VisionComponent.h"
#include "model/entity/components/WeaponComponent.h"
#include "model/entity/entities/items/Item.h"
#include "model/entity/entities/items/Weapon.h"
#include "model/repository/cells/Floor.h"
#include "model/repository/cells/Glass.h"
#include "model/repository/cells/Partition.h"
#include "model/repository/cells/Stash.h"
#include "model/repository/cells/Wall.h"

#include <fstream>
#include <iomanip>
#include <string_view>

namespace game::service {

constexpr std::string_view kHeader = "MYGAME_SAVE";
constexpr int kVersion = 1;

std::string LoadSaveService::ammo_type_string(game::AmmoType type) {
    switch (type) {
        case game::AmmoType::SHOTGUN: return "SHOTGUN";
        case game::AmmoType::RIFLE: return "RIFLE";
        case game::AmmoType::PISTOL: return "PISTOL";
        case game::AmmoType::SNIPER: return "SNIPER";
    }
    return "PISTOL";
}

bool LoadSaveService::parse_ammo_type(const std::string& token, game::AmmoType& out) {
    if (token == "SHOTGUN") { out = game::AmmoType::SHOTGUN; return true; }
    if (token == "RIFLE") { out = game::AmmoType::RIFLE; return true; }
    if (token == "PISTOL") { out = game::AmmoType::PISTOL; return true; }
    if (token == "SNIPER") { out = game::AmmoType::SNIPER; return true; }
    return false;
}

std::string LoadSaveService::behavior_string(game::entity::components::AIBehavior behavior) {
    using game::entity::components::AIBehavior;
    switch (behavior) {
        case AIBehavior::WILD: return "WILD";
        case AIBehavior::INTELLIGENT: return "INTELLIGENT";
        case AIBehavior::FORAGER: return "FORAGER";
    }
    return "WILD";
}

bool LoadSaveService::parse_behavior(const std::string& token, game::entity::components::AIBehavior& out) {
    using game::entity::components::AIBehavior;
    if (token == "WILD") { out = AIBehavior::WILD; return true; }
    if (token == "INTELLIGENT") { out = AIBehavior::INTELLIGENT; return true; }
    if (token == "FORAGER") { out = AIBehavior::FORAGER; return true; }
    return false;
}

std::string LoadSaveService::cell_type_string(const game::repo::cells::ICell& cell) {
    if (dynamic_cast<const game::repo::cells::Stash*>(&cell)) return "STASH";
    if (dynamic_cast<const game::repo::cells::Floor*>(&cell)) return "FLOOR";
    if (dynamic_cast<const game::repo::cells::Wall*>(&cell)) return "WALL";
    if (dynamic_cast<const game::repo::cells::Glass*>(&cell)) return "GLASS";
    if (dynamic_cast<const game::repo::cells::Partition*>(&cell)) return "PARTITION";
    return "UNKNOWN";
}

std::unique_ptr<game::repo::cells::ICell> LoadSaveService::make_cell(const std::string& type, bool broken) {
    if (type == "FLOOR") return std::make_unique<game::repo::cells::Floor>();
    if (type == "WALL") return std::make_unique<game::repo::cells::Wall>();
    if (type == "GLASS") {
        auto cell = std::make_unique<game::repo::cells::Glass>();
        if (broken) cell->break_glass();
        return cell;
    }
    if (type == "STASH") return std::make_unique<game::repo::cells::Stash>();
    if (type == "PARTITION") {
        auto cell = std::make_unique<game::repo::cells::Partition>();
        if (broken) cell->break_partition();
        return cell;
    }
    return nullptr;
}

void LoadSaveService::save_item(std::ostream& out, const game::entity::items::Item& item) {
    if (auto* med = dynamic_cast<const game::entity::items::Medkit*>(&item)) {
        out << "ITEM MEDKIT " << med->get_id() << " " << med->get_weight() << " "
            << med->get_heal() << " " << med->get_cost() << "\n";
        return;
    }
    if (auto* bag = dynamic_cast<const game::entity::items::AmmoBag*>(&item)) {
        out << "ITEM AMMOBAG " << bag->get_id() << " " << bag->get_weight() << " "
            << bag->get_current_ammo() << " " << bag->get_max_ammo() << " "
            << ammo_type_string(bag->get_ammo_type()) << "\n";
        return;
    }
    if (auto* wp = dynamic_cast<const game::entity::items::Weapon*>(&item)) {
        auto dmg = wp->get_dmg();
        out << "ITEM WEAPON " << wp->get_id() << " " << wp->get_weight() << " "
            << dmg.min_dmg << " " << dmg.max_dmg << " "
            << wp->get_range() << " " << wp->get_attack_cost() << " " << wp->get_reload_cost() << " "
            << ammo_type_string(wp->get_ammo_type()) << " " << wp->get_current_ammo() << " "
            << wp->get_max_ammo() << "\n";
        return;
    }
}

std::unique_ptr<game::entity::items::Item> LoadSaveService::load_item(std::istream& in) {
    std::string kind;
    if (!(in >> kind)) return nullptr;
    if (kind == "MEDKIT") {
        game::ItemId id = 0;
        int weight = 0;
        int heal = 0;
        int cost = 0;
        if (!(in >> id >> weight >> heal >> cost)) return nullptr;
        return std::make_unique<game::entity::items::Medkit>(id, weight, heal, cost);
    }
    if (kind == "AMMOBAG") {
        game::ItemId id = 0;
        int weight = 0;
        int cur = 0;
        int maxv = 0;
        std::string ammo;
        if (!(in >> id >> weight >> cur >> maxv >> ammo)) return nullptr;
        game::AmmoType type{};
        if (!parse_ammo_type(ammo, type)) return nullptr;
        return std::make_unique<game::entity::items::AmmoBag>(id, weight, cur, maxv, type);
    }
    if (kind == "WEAPON") {
        game::ItemId id = 0;
        int weight = 0;
        int min_dmg = 1;
        int max_dmg = 1;
        int range = 0;
        int attack_cost = 0;
        int reload_cost = 0;
        std::string ammo;
        int cur = 0;
        int maxv = 0;
        if (!(in >> id >> weight >> min_dmg >> max_dmg >> range >> attack_cost
            >> reload_cost >> ammo >> cur >> maxv)) return nullptr;
        game::AmmoType type{};
        if (!parse_ammo_type(ammo, type)) return nullptr;
        return std::make_unique<game::entity::items::Weapon>(
            id, weight, game::Damage{min_dmg, max_dmg}, range, attack_cost, reload_cost, type, cur, maxv);
    }
    return nullptr;
}

bool LoadSaveService::save(const World& world, const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;

    auto* level = world.get_level();
    if (!level) return false;

    out << kHeader << " " << kVersion << "\n";
    out << "LEVEL " << level->get_id() << " " << std::quoted(level->get_name()) << " "
        << level->get_width() << " " << level->get_height() << "\n";

    for (int x = 0; x < static_cast<int>(level->get_width()); ++x) {
        for (int y = 0; y < static_cast<int>(level->get_height()); ++y) {
            auto* cell = level->get_cell({x, y});
            if (!cell) return false;
            std::string type = cell_type_string(*cell);
            bool broken = false;
            if (auto* glass = dynamic_cast<const game::repo::cells::Glass*>(cell)) broken = glass->is_broken();
            if (auto* part = dynamic_cast<const game::repo::cells::Partition*>(cell)) broken = part->is_broken();

            std::vector<const game::entity::items::Item*> items;
            if (auto* cont = dynamic_cast<const game::repo::cells::IItemContainer*>(cell)) {
                items = cont->get_items();
            }
            out << "CELL " << x << " " << y << " " << type << " " << (broken ? 1 : 0)
                << " " << items.size() << "\n";
            for (const auto* item : items) {
                if (item) save_item(out, *item);
            }
        }
    }

    auto entities = level->get_entities();
    out << "ENTITIES " << entities.size() << "\n";
    for (const auto* entity : entities) {
        if (!entity) continue;
        auto* pos = level->get_entity_position(entity->get_id());
        if (!pos) return false;
        out << "ENTITY " << entity->get_id() << " " << std::quoted(entity->get_name()) << " "
            << entity->get_team_id() << " " << pos->x << " " << pos->y << "\n";

        std::vector<std::string> comps;
        if (entity->get_component<entity::components::HealthComponent>()) comps.emplace_back("HEALTH");
        if (entity->get_component<entity::components::InventoryComponent>()) comps.emplace_back("INVENTORY");
        if (entity->get_component<entity::components::MoveComponent>()) comps.emplace_back("MOVE");
        if (entity->get_component<entity::components::TimePointsComponent>()) comps.emplace_back("TIMEPOINTS");
        if (entity->get_component<entity::components::CombatComponent>()) comps.emplace_back("COMBAT");
        if (entity->get_component<entity::components::MeleeComponent>()) comps.emplace_back("MELEE");
        if (entity->get_component<entity::components::AIComponent>()) comps.emplace_back("AI");
        if (entity->get_component<entity::components::WeaponComponent>()) comps.emplace_back("WEAPON");
        if (entity->get_component<entity::components::VisionComponent>()) comps.emplace_back("VISION");

        out << "COMPONENTS " << comps.size() << "\n";
        for (const auto& comp : comps) {
            if (comp == "HEALTH") {
                auto* c = entity->get_component<entity::components::HealthComponent>();
                out << "COMP HEALTH " << c->get_current_hp() << " " << c->get_max_hp() << "\n";
            } else if (comp == "INVENTORY") {
                auto* c = entity->get_component<entity::components::InventoryComponent>();
                out << "COMP INVENTORY " << c->get_capacity() << " " << c->get_max_weight() << " "
                    << c->size() << "\n";
                for (const auto* item : c->get_items()) {
                    if (item) save_item(out, *item);
                }
            } else if (comp == "MOVE") {
                auto* c = entity->get_component<entity::components::MoveComponent>();
                out << "COMP MOVE " << c->get_step_cost() << "\n";
            } else if (comp == "TIMEPOINTS") {
                auto* c = entity->get_component<entity::components::TimePointsComponent>();
                out << "COMP TIMEPOINTS " << c->get_current_points() << " " << c->get_max_points() << "\n";
            } else if (comp == "COMBAT") {
                auto* c = entity->get_component<entity::components::CombatComponent>();
                out << "COMP COMBAT " << c->get_base_accuracy() << "\n";
            } else if (comp == "MELEE") {
                auto* c = entity->get_component<entity::components::MeleeComponent>();
                out << "COMP MELEE " << c->get_damage() << " " << c->get_attack_cost() << "\n";
            } else if (comp == "AI") {
                auto* c = entity->get_component<entity::components::AIComponent>();
                out << "COMP AI " << behavior_string(c->get_behavior()) << "\n";
            } else if (comp == "WEAPON") {
                auto* c = entity->get_component<entity::components::WeaponComponent>();
                auto* wp = c->get_weapon();
                if (wp) {
                    auto dmg = wp->get_dmg();
                    out << "COMP WEAPON " << wp->get_id() << " " << wp->get_weight() << " "
                        << dmg.min_dmg << " " << dmg.max_dmg << " "
                        << wp->get_range() << " " << wp->get_attack_cost() << " " << wp->get_reload_cost()
                        << " " << ammo_type_string(wp->get_ammo_type()) << " " << wp->get_current_ammo()
                        << " " << wp->get_max_ammo() << "\n";
                } else {
                    out << "COMP WEAPON NONE\n";
                }
            } else if (comp == "VISION") {
                auto* c = entity->get_component<entity::components::VisionComponent>();
                out << "COMP VISION " << c->get_vision_radius() << " " << (c->is_sees_items_only() ? 1 : 0) << "\n";
            }
        }
        out << "ENDENTITY\n";
    }

    out << "END\n";
    return out.good();
}

std::unique_ptr<World> LoadSaveService::load(const std::string& path) const {
    std::ifstream in(path);
    if (!in) return nullptr;

    std::string header;
    int version = 0;
    if (!(in >> header >> version)) return nullptr;
    if (header != kHeader || version != kVersion) return nullptr;

    std::string level_tag;
    game::LevelId level_id = 0;
    std::string level_name;
    size_t width = 0;
    size_t height = 0;
    if (!(in >> level_tag >> level_id >> std::quoted(level_name) >> width >> height)) return nullptr;
    if (level_tag != "LEVEL") return nullptr;

    auto level = std::make_unique<game::repo::Level>(level_id, level_name);
    level->resize_field(width, height);

    for (int x = 0; x < static_cast<int>(width); ++x) {
        for (int y = 0; y < static_cast<int>(height); ++y) {
            std::string cell_tag;
            std::string type;
            int broken = 0;
            size_t items_count = 0;
            int cx = 0;
            int cy = 0;
            if (!(in >> cell_tag >> cx >> cy >> type >> broken >> items_count)) return nullptr;
            if (cell_tag != "CELL") return nullptr;

            auto cell = make_cell(type, broken != 0);
            if (!cell) return nullptr;
            auto old = level->set_cell({cx, cy}, std::move(cell));
            (void)old;

            if (items_count > 0) {
                auto* cont = dynamic_cast<game::repo::cells::IItemContainer*>(level->get_cell({cx, cy}));
                if (!cont) return nullptr;
                for (size_t i = 0; i < items_count; ++i) {
                    std::string item_tag;
                    if (!(in >> item_tag)) return nullptr;
                    if (item_tag != "ITEM") return nullptr;
                    auto item = load_item(in);
                    if (!item) return nullptr;
                    cont->add(std::move(item));
                }
            }
        }
    }

    std::string entities_tag;
    size_t entity_count = 0;
    if (!(in >> entities_tag >> entity_count)) return nullptr;
    if (entities_tag != "ENTITIES") return nullptr;

    for (size_t i = 0; i < entity_count; ++i) {
        std::string ent_tag;
        game::EntityId id = 0;
        std::string name;
        game::TeamId team = 0;
        int x = 0;
        int y = 0;
        if (!(in >> ent_tag >> id >> std::quoted(name) >> team >> x >> y)) return nullptr;
        if (ent_tag != "ENTITY") return nullptr;

        auto ent = std::make_unique<game::entity::Entity>(id, name, team);

        std::string comps_tag;
        size_t comps_count = 0;
        if (!(in >> comps_tag >> comps_count)) return nullptr;
        if (comps_tag != "COMPONENTS") return nullptr;

        for (size_t c = 0; c < comps_count; ++c) {
            std::string comp_tag;
            std::string comp_type;
            if (!(in >> comp_tag >> comp_type)) return nullptr;
            if (comp_tag != "COMP") return nullptr;

            if (comp_type == "HEALTH") {
                int cur = 0;
                int maxv = 0;
                if (!(in >> cur >> maxv)) return nullptr;
                ent->add_component<entity::components::HealthComponent, entity::components::DefaultHealthComp>(cur, maxv);
            } else if (comp_type == "INVENTORY") {
                int capacity = 0;
                int max_weight = 0;
                size_t item_count = 0;
                if (!(in >> capacity >> max_weight >> item_count)) return nullptr;
                auto& inv = ent->add_component<entity::components::InventoryComponent,
                    entity::components::DefaultInventoryComp>(capacity, max_weight);
                for (size_t n = 0; n < item_count; ++n) {
                    std::string item_tag;
                    if (!(in >> item_tag)) return nullptr;
                    if (item_tag != "ITEM") return nullptr;
                    auto item = load_item(in);
                    if (!item) return nullptr;
                    inv.add(std::move(item));
                }
            } else if (comp_type == "MOVE") {
                int cost = 0;
                if (!(in >> cost)) return nullptr;
                ent->add_component<entity::components::MoveComponent, entity::components::DefaultMoveComp>(cost);
            } else if (comp_type == "TIMEPOINTS") {
                int cur = 0;
                int maxv = 0;
                if (!(in >> cur >> maxv)) return nullptr;
                ent->add_component<entity::components::TimePointsComponent,
                    entity::components::DefaultTimePointsComp>(cur, maxv);
            } else if (comp_type == "COMBAT") {
                double acc = 0.0;
                if (!(in >> acc)) return nullptr;
                ent->add_component<entity::components::CombatComponent,
                    entity::components::DefaultCombatComp>(acc);
            } else if (comp_type == "MELEE") {
                int dmg = 0;
                int cost = 0;
                if (!(in >> dmg >> cost)) return nullptr;
                ent->add_component<entity::components::MeleeComponent,
                    entity::components::DefaultMeleeComp>(dmg, cost);
            } else if (comp_type == "AI") {
                std::string behavior;
                if (!(in >> behavior)) return nullptr;
                entity::components::AIBehavior b{};
                if (!parse_behavior(behavior, b)) return nullptr;
                ent->add_component<entity::components::AIComponent,
                    entity::components::DefaultAIComp>(b);
            } else if (comp_type == "WEAPON") {
                std::string maybe_none;
                if (!(in >> maybe_none)) return nullptr;
                if (maybe_none == "NONE") {
                    ent->add_component<entity::components::WeaponComponent,
                        entity::components::DefaultWeaponComp>(std::unique_ptr<game::entity::items::Weapon>{});
                } else {
                    game::ItemId wid = 0;
                    int weight = 0;
                    int min_dmg = 1;
                    int max_dmg = 1;
                    int range = 0;
                    int attack_cost = 0;
                    int reload_cost = 0;
                    std::string ammo;
                    int cur = 0;
                    int maxv = 0;
                    try {
                        wid = static_cast<game::ItemId>(std::stoull(maybe_none));
                    } catch (const std::exception&) {
                        return nullptr;
                    }
                    if (!(in >> weight >> min_dmg >> max_dmg >> range >> attack_cost >> reload_cost
                        >> ammo >> cur >> maxv)) return nullptr;
                    game::AmmoType type{};
                    if (!parse_ammo_type(ammo, type)) return nullptr;
                    auto weapon = std::make_unique<game::entity::items::Weapon>(
                        wid, weight, game::Damage{min_dmg, max_dmg}, range, attack_cost, reload_cost, type, cur, maxv);
                    ent->add_component<entity::components::WeaponComponent,
                        entity::components::DefaultWeaponComp>(std::move(weapon));
                }
            } else if (comp_type == "VISION") {
                int radius = 0;
                int sees_only = 0;
                if (!(in >> radius >> sees_only)) return nullptr;
                ent->add_component<entity::components::VisionComponent,
                    entity::components::DefaultVisionComp>(radius, sees_only != 0);
            } else {
                return nullptr;
            }
        }

        std::string end_tag;
        if (!(in >> end_tag)) return nullptr;
        if (end_tag != "ENDENTITY") return nullptr;

        if (!level->spawn_entity(std::move(ent), {x, y})) return nullptr;
    }

    std::string end_tag;
    if (!(in >> end_tag)) return nullptr;
    if (end_tag != "END") return nullptr;

    return std::make_unique<World>(std::move(level));
}

}
