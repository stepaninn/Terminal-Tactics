#ifndef MYGAMEPROJECT_STASH_H
#define MYGAMEPROJECT_STASH_H
#include "Cell.h"
#include "Floor.h"

namespace game::repo::cells {

/// @brief Класс клетки тайника
class Stash final : public Floor {
public:
    Stash() = default;

    /**
     * @brief Метод получения отображаемого имени клетки
     * @return std::string_view имя клетки
     */
    [[nodiscard]] std::string_view view_name() const noexcept override { return "Stash"; }
};

}

#endif //MYGAMEPROJECT_STASH_H
