#ifndef INC_3_ICOMPONENT_H
#define INC_3_ICOMPONENT_H

namespace game::entity::components {

/// @brief Класс-интерфейс компонента
class IComponent {
public:
    virtual ~IComponent() = default;
};

}

#endif
