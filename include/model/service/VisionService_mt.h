#ifndef MYGAMEPROJECT_VISIONSERVICE_MT_H
#define MYGAMEPROJECT_VISIONSERVICE_MT_H

#include "ServiceBase_mt.h"
#include "model/service/World_mt.h"

namespace game::mt::repo {
class Level;
}

namespace game::mt::service {

/// @brief Сервис видимости
class VisionService : public ServiceBase {
public:
    explicit VisionService(std::shared_ptr<events::EventBus> bus = nullptr)
        : ServiceBase(std::move(bus)) {}

    /**
     * @brief Метод обновления видимости отдельного существа
     * @param w Мир
     * @param id ID сущности
     */
    static void update_unit_fov(game::mt::service::World& w, game::mt::EntityId id);

    /**
     * @brief Метод обновления видимых ячеек для команды
     * @param w Мир
     * @param team Команда
     */
    static void rebuild_team_visible(game::mt::service::World& w, game::mt::TeamId team);

    /**
     * @brief Метод обновления исследованных ячеек для команды
     * @param w Мир
     * @param team Команда
     */
    static void update_team_explored(game::mt::service::World& w, game::mt::TeamId team);

    /**
     * @brief Метод перестройки видимости всех существ на карте
     * @param w Мир
     */
    void rebuild_all_fov(game::mt::service::World& w);

    /**
     * @brief Метод проверки возможности попадания
     * @param lvl Уровень
     * @param from Позиция начала стрельбы
     * @param to Конечная точка стрельбы
     * @return bool true, если попадание возможно
     */
    [[nodiscard]] static bool has_line_of_fire(const game::mt::repo::Level& lvl, game::mt::Position from, game::mt::Position to);

    /**
     * @brief Метод получения видимых существ
     * @param w Мир
     * @param observer_id ID наблюдателя
     * @return Массив сущностей, которые видно радиус
     */
    [[nodiscard]] static std::vector<EntityId> visible_entities(game::mt::service::World& w, game::mt::EntityId observer_id);

private:
    static void cast_light(VisibilityMap& map, const game::mt::repo::Level& lvl, int x, int y, int radius, int row,
        double start_slope, double end_slope, int xx, int xy, int yx, int yy);

    [[nodiscard]] static VisibilityMap compute_fov(const game::mt::repo::Level& lvl, Position pos, int r);
};

}

#endif //MYGAMEPROJECT_VISIONSERVICE_MT_H
