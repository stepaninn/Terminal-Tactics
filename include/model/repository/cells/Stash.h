#ifndef MYGAMEPROJECT_STASH_H
#define MYGAMEPROJECT_STASH_H
#include "Cell.h"
#include "Floor.h"

namespace game::repo::cells {

class Stash final : public Floor {
public:
    Stash() = default;

    [[nodiscard]] std::string_view view_name() const noexcept override { return "Stash"; }
};

}

#endif //MYGAMEPROJECT_STASH_H
