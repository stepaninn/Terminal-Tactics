#ifndef INC_3_ENTITY_H
#define INC_3_ENTITY_H

#include "types.h"
#include "components/IComponents.h"

#include <memory>
#include <string>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace game {

class Entity {
public:
    Entity() = default;
    explicit Entity(id_t id, std::string name = {}) : id_(id), name_(std::move(name)) {}
    virtual ~Entity() = default;

    [[nodiscard]] id_t get_id() const noexcept { return id_; }
    [[nodiscard]] const std::string& get_name() const noexcept { return name_; }
    void set_name(std::string new_name) { name_ = std::move(new_name); }

    void add_component(std::shared_ptr<IComponent> comp);

    // возвращает удалённый компонент
    std::shared_ptr<IComponent> remove_component(std::shared_ptr<IComponent> comp);

    template<typename T>
    [[nodiscard]] std::shared_ptr<T> get_component() const {
        std::type_index idx(typeid(T));
        auto it = components_.find(idx);
        if (it == components_.end()) {
            for (const auto &p : components_) {
                std::shared_ptr<T> casted = std::dynamic_pointer_cast<T>(p.second);
                if (casted) return casted;
            }
            return nullptr;
        }
        return std::dynamic_pointer_cast<T>(it->second);
    }

    [[nodiscard]] std::vector<std::shared_ptr<IComponent>> get_components() const;

protected:
    id_t id_ = 0;
    std::string name_;
    std::map<std::type_index, std::shared_ptr<IComponent>> components_;
};

}

#endif //INC_3_ENTITY_H