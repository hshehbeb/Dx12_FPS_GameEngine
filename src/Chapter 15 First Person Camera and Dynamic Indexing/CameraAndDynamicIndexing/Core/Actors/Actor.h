#pragma once
#include <memory>
#include <vector>
#include "../Components/IComponent.h"
#include "../../../../Common/GameTimer.h"


// Actor gives you ability to attach components 
class Actor
{
public:
    Actor(std::vector<std::shared_ptr<IComponent>>&& components);
    Actor(const Actor& other) = delete; /* not impl yet */
    Actor(Actor&& other) = delete;
    ~Actor() = default;

    void Initialize(ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList);
    void Update(const GameTimer& gt);
    
    template <class T>
    bool TryGetComponent(T** result);

protected:
    typedef std::vector<std::shared_ptr<IComponent>> t_ComponentsRegistry; 
    
    t_ComponentsRegistry mComponents {};
};

template <class T>
bool Actor::TryGetComponent(T** result)
{
    for (auto& comp : mComponents)
    {
        if (dynamic_cast<T*>(comp.get()))
        {
            *result = static_cast<T*>(comp.get());
            return true;
        }
    }

    *result = nullptr;
    return false;
}