// include/esc.h

#pragma once
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <vector>      
#include <unordered_set> 
#include "components.h"     

using Entity = size_t;

class ECS {
public:
    Entity createEntity() {
        return nextEntity++;
    }

    template<typename T>
    void addComponent(Entity e, T comp) {
        auto& map = getComponentMap<T>();
        map[e] = comp;
    }

    template<typename T>
    T* getComponent(Entity e) {
        auto& map = getComponentMap<T>();
        auto it = map.find(e);
        if (it != map.end()) return &it->second;
        return nullptr;
    }

    template<typename T>
    std::unordered_map<Entity, T>& getComponentMap() {
        static std::unordered_map<Entity, T> map;
        return map;
    }

    template<typename T>
    void removeComponent(Entity e) {
        auto& map = getComponentMap<T>();
        map.erase(e);
    }

    template<typename T>
    bool hasComponent(Entity e) {
        auto& map = getComponentMap<T>();
        return map.find(e) != map.end();
    }

    std::vector<Entity> getAllEntities() {
        std::unordered_set<Entity> uniqueEntities;

        auto mergeMap = [&](auto& map) {
            for (auto& [e, _] : map) uniqueEntities.insert(e);
        };
        mergeMap(getComponentMap<Position>());
        mergeMap(getComponentMap<Sprite>());  
        mergeMap(getComponentMap<InputControlled>());
        mergeMap(getComponentMap<AIPatrol>());

        return std::vector<Entity>(uniqueEntities.begin(), uniqueEntities.end());
    }

    

private:
    Entity nextEntity = 0;

    
};


