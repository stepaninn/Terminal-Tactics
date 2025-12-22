#ifndef INC_3_ENTITY_H
#define INC_3_ENTITY_H

#include "../components/IComponent.h"
#include "../../../types.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <ranges>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace game {

class Entity {
public:
    Entity() = default;
    explicit Entity(id_t id, std::string name) : id_(id), name_(std::move(name)) {}
    virtual ~Entity() = default;

    [[nodiscard]] id_t get_id() const noexcept { return id_; }
    [[nodiscard]] const std::string& get_name() const noexcept { return name_; }
    void set_name(std::string new_name) { name_ = std::move(new_name); }
    void set_id(id_t id) { id_ = id; }

    template<typename Key, typename Impl = Key, typename... Args>
    requires std::is_base_of_v<IComponent, Key> && std::is_base_of_v<Key, Impl>
    Key& add_component(Args&&... args) {
        auto comp = std::make_unique<Impl>(std::forward<Args>(args)...);
        Key& ref = *comp;
        components_[std::type_index(typeid(Key))] = std::move(comp);
        return ref;
    }

    template<typename Key>
    requires std::is_base_of_v<IComponent, Key>
    std::unique_ptr<Key> remove_component() {
        auto it = components_.find(std::type_index(typeid(Key)));
        if (it == components_.end()) return nullptr;
        auto raw = static_cast<Key*>(it->second.release());
        components_.erase(it);
        return std::unique_ptr<Key>(raw);
    }

    template<typename Key>
    requires std::is_base_of_v<IComponent, Key>
    Key* get_component() const noexcept {
        auto it = components_.find(std::type_index(typeid(Key)));
        if (it == components_.end()) return nullptr;
        return static_cast<Key*>(it->second.get());
    }

protected:
    id_t id_{};
    std::string name_;
    std::unordered_map<std::type_index, std::unique_ptr<IComponent>> components_;
};

}

#endif //INC_3_ENTITY_H