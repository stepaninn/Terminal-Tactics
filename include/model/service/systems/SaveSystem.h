#ifndef INC_3_SAVESYSTEM_H
#define INC_3_SAVESYSTEM_H

#include "ISystem.h"
#include <string>

namespace game {

class SaveSystem : public DefaultSystem {
public:
    explicit SaveSystem(std::shared_ptr<EventBus> bus) : DefaultSystem(bus) {}

    [[nodiscard]] std::string get_save_dir() const { return save_dir_; }
    void set_save_dir(std::string dir);

    [[nodiscard]] std::shared_ptr<Level> load_save(const std::string& dir);
    void save_game(std::shared_ptr<Level> game);

    void update(std::shared_ptr<Level> lvl) override;
private:
    std::string save_dir_;
};

} // namespace game

#endif //INC_3_SAVESYSTEM_H