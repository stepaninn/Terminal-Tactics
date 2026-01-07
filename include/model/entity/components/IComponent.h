#ifndef MYGAMEPROJECT_ICOMPONENT_H
#define MYGAMEPROJECT_ICOMPONENT_H

namespace game::entity::components {

/// @brief Класс-интерфейс компонента
class IComponent {
public:
    virtual ~IComponent() = default;
};

}

#endif
