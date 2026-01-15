#ifndef MYGAMEPROJECT_DESTRUCTIBLECELL_H
#define MYGAMEPROJECT_DESTRUCTIBLECELL_H

namespace game::repo::cells {

/// @brief Интерфейс разрушаемой клетки
class IDestructibleCell {
public:
    virtual ~IDestructibleCell() = default;

    /**
     * @brief Метод проверки возможности разрушения выстрелом
     * @return bool true, если выстрел может разрушить клетку
     */
    [[nodiscard]] virtual bool can_be_shot() const noexcept = 0;
    /**
     * @brief Метод применения выстрела к клетке
     * @return bool true, если состояние клетки изменилось
     */
    [[nodiscard]] virtual bool apply_shot() noexcept = 0;
};

}

#endif //MYGAMEPROJECT_DESTRUCTIBLECELL_H
