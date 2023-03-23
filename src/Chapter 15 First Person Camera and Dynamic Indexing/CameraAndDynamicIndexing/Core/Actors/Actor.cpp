#include "Actor.h"

Actor::Actor(std::vector<std::shared_ptr<IComponent>>&& components)
{
    mComponents = std::move(components);
}

void Actor::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList)
{
    for (auto& comp : mComponents)
    {
        comp->Initialize(device, cmdList, rItemsList, this);
    }
}

void Actor::Update()
{
    for (auto& comp : mComponents)
    {
        comp->Update(this);
    }
}
