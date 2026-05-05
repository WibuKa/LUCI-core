#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>

class Component;
class Entity;

using ComponentID = std::size_t;

inline ComponentID getComponentTypeID(){
    static ComponentID lastID = 0;
    return lastID++;
}

template <typename T>
inline ComponentID getComponentTypeID() noexcept
{
    static ComponentID typeID = getComponentTypeID();
    return typeID;
}

constexpr std::size_t maxComponents = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using ComponentArray = std::array<Component*, maxComponents>;

class Component
{
public:
    Entity* entity;
    int ID;

    virtual void init(){}
    virtual void update(double delta_time){}
    virtual void draw(){}

    virtual ~Component(){}
    template<typename T>
    T& getComponent();

};

class Entity
{
private:
    std::vector<std::unique_ptr<Component>> components;

    ComponentArray componentArray;
    ComponentBitSet componentBitSet;
public:
    int ID;
    bool active = false;
    const char* name;

    void setID(int _id){
        ID = _id;
        
    }

    void update(double delta_time)
    {
        for (auto& c: components) c-> update(delta_time);
        for (auto& c: components) c-> draw();
    }
    void draw(){}
    bool isActive() const{return active;}
    void destroy() {active = false;}

    template <typename T> bool hasComponent() const{
        return componentBitSet[getComponentTypeID<T>()];
    }

    template <typename T> ComponentID getComponentID() const {
        return getComponentTypeID<T>();
    }

    template <typename T,typename... Targs>
    T& addComponent(Targs&&... mArgs)
    {
        T* c(new T(std::forward<Targs>(mArgs)...));
        c->entity = this;
        c->ID = ID;
        std::unique_ptr<Component> uPtr{c};
        components.emplace_back(std::move(uPtr));

        componentArray[getComponentTypeID<T>()] = c;
        componentBitSet[getComponentTypeID<T>()] = true;

        c->init();
        return *c;
    }

    template<typename T> T& getComponent() const
    {
        auto ptr(componentArray[getComponentTypeID<T>()]);
        return * static_cast<T*>(ptr);
    }
};

template<typename T>
T& Component::getComponent() {
    return entity->template getComponent<T>();
}

class Manager
{
private:
    std::vector<std::unique_ptr<Entity>> entities;
public:
    void update(double delta_time)
    {
        for (auto& e : entities) e->update(delta_time);
    }
    void draw()
    {
        for (auto& e : entities) e->draw();
    }
    void refresh()
    {
        entities.erase(std::remove_if(std::begin(entities), std::end(entities),
            [](const std::unique_ptr<Entity> &mEntity)
        {
            return !mEntity ->isActive();
        }),
            std::end(entities));
    }

    Entity& addEntity()
    {
        Entity* e = new Entity();
        std::unique_ptr<Entity> uPtr{e};
        entities.emplace_back(std::move(uPtr));
        return *e;
    }
};
