#ifndef MYGAMEPROJECT_ENTITY_MT_H
#define MYGAMEPROJECT_ENTITY_MT_H

#include "../components/IComponent_mt.h"
#include "../../../types_mt.h"

#include <memory>
#include <mutex>
#include <string>
#include <tbb/concurrent_hash_map.h>
#include <typeindex>
#include <typeinfo>

namespace game::mt::entity {

/// @brief Базовая сущность игры с набором компонентов
class Entity {
public:
    Entity() = default;
    explicit Entity(game::mt::EntityId id, std::string name = {}, game::mt::TeamId team_id = 0) :
        id_(id), name_(std::move(name)), team_id_(team_id) {}
    virtual ~Entity() = default;

    /**
     * @brief Метод получения идентификатора сущности
     * @return EntityId идентификатор сущности
     */
    [[nodiscard]] game::mt::EntityId get_id() const noexcept {
        std::lock_guard<std::mutex> lock(info_mutex_);
        return id_;
    }
    /**
     * @brief Метод получения имени сущности
     * @return const std::string& имя сущности
     */
    [[nodiscard]] const std::string& get_name() const noexcept {
        std::lock_guard<std::mutex> lock(info_mutex_);
        return name_;
    }
    /**
     * @brief Метод получения идентификатора команды
     * @return TeamId идентификатор команды
     */
    [[nodiscard]] game::mt::TeamId get_team_id() const noexcept {
        std::lock_guard<std::mutex> lock(info_mutex_);
        return team_id_;
    }
    /**
     * @brief Метод задания имени сущности
     * @param new_name новое имя сущности
     */
    void set_name(std::string new_name) {
        std::lock_guard<std::mutex> lock(info_mutex_);
        name_ = std::move(new_name);
    }
    /**
     * @brief Метод задания идентификатора сущности
     * @param id новый идентификатор сущности
     */
    void set_id(game::mt::EntityId id) {
        std::lock_guard<std::mutex> lock(info_mutex_);
        id_ = id;
    }
    /**
     * @brief Метод задания идентификатора команды
     * @param team_id новый идентификатор команды
     */
    void set_team_id(game::mt::TeamId team_id) noexcept {
        std::lock_guard<std::mutex> lock(info_mutex_);
        team_id_ = team_id;
    }

    template<typename Key>
    requires std::is_base_of_v<components::IComponent, Key>
    /**
     * @brief Метод проверки наличия компонента
     * @tparam Key тип компонента
     * @return bool true, если компонент установлен
     */
    [[nodiscard]] bool has_component() const noexcept {
        tbb::concurrent_hash_map<std::type_index, std::shared_ptr<components::IComponent>>::const_accessor acc;
        return components_.find(acc, std::type_index(typeid(Key)));
    }

    template<typename Key, typename Impl = Key, typename... Args>
    requires std::is_base_of_v<components::IComponent, Key> && std::is_base_of_v<Key, Impl>
    /**
     * @brief Метод добавления компонента
     * @tparam Key тип компонента
     * @tparam Impl тип реализации компонента
     * @param args аргументы конструктора компонента
     * @return Key& ссылка на добавленный компонент
     * @note Если компонент уже есть, возвращает существующий
     */
    Key& add_component(Args&&... args) {
        auto type = std::type_index(typeid(Key));
        tbb::concurrent_hash_map<std::type_index, std::shared_ptr<components::IComponent>>::accessor acc;
        if (components_.find(acc, type)) {
            return *static_cast<Key*>(acc->second.get());
        }

        auto comp = std::make_shared<Impl>(std::forward<Args>(args)...);
        Key& ref = *comp;
        if (components_.insert(acc, type)) {
            acc->second = comp;
        } else {
            return *static_cast<Key*>(acc->second.get());
        }
        return ref;
    }

    template<typename Key, typename Impl = Key, typename... Args>
    requires std::is_base_of_v<components::IComponent, Key> && std::is_base_of_v<Key, Impl>
    /**
     * @brief Метод замены компонента
     * @tparam Key тип компонента
     * @tparam Impl тип реализации компонента
     * @param args аргументы конструктора нового компонента
     * @return std::unique_ptr<Key> прежний компонент или nullptr
     */
    std::shared_ptr<Key> replace_component(Args&&... args) {
        std::lock_guard<std::mutex> lock(components_mutex_);
        auto type = std::type_index(typeid(Key));
        auto comp = std::make_shared<Impl>(std::forward<Args>(args)...);
        std::shared_ptr<Key> old;
        tbb::concurrent_hash_map<std::type_index, std::shared_ptr<components::IComponent>>::accessor acc;
        if (components_.find(acc, type)) {
            old = std::static_pointer_cast<Key>(acc->second);
            components_.erase(acc);
        }
        if (components_.insert(acc, type)) {
            acc->second = comp;
        }
        return old;
    }

    template<typename Key>
    requires std::is_base_of_v<components::IComponent, Key>
    /**
     * @brief Метод удаления компонента
     * @tparam Key тип компонента
     * @return std::unique_ptr<Key> удаленный компонент или nullptr
     */
    std::shared_ptr<Key> remove_component() {
        std::lock_guard<std::mutex> lock(components_mutex_);
        auto type = std::type_index(typeid(Key));
        tbb::concurrent_hash_map<std::type_index, std::shared_ptr<components::IComponent>>::accessor acc;
        if (!components_.find(acc, type)) return nullptr;
        auto res = std::static_pointer_cast<Key>(acc->second);
        components_.erase(acc);
        return res;
    }

    template<typename Key>
    requires std::is_base_of_v<components::IComponent, Key>
    /**
     * @brief Метод получения компонента
     * @tparam Key тип компонента
     * @return Key* указатель на компонент или nullptr
     */
    [[nodiscard]] Key* get_component() const noexcept {
        tbb::concurrent_hash_map<std::type_index, std::shared_ptr<components::IComponent>>::const_accessor acc;
        if (!components_.find(acc, std::type_index(typeid(Key)))) return nullptr;
        return static_cast<Key*>(acc->second.get());
    }

protected:
    mutable std::mutex info_mutex_;
    mutable std::mutex components_mutex_;
    game::mt::EntityId id_{};
    std::string name_;
    game::mt::TeamId team_id_{};
    tbb::concurrent_hash_map<std::type_index, std::shared_ptr<components::IComponent>> components_;
};

}

#endif //MYGAMEPROJECT_ENTITY_MT_H
