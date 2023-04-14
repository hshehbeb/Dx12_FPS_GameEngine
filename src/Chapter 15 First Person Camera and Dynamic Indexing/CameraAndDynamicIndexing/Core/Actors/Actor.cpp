#include "Actor.h"

Actor::Actor(std::vector<std::shared_ptr<IComponent>>&& components)
{
    mComponents = std::move(components);
}

void Actor::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList)
{
    for (auto& comp : mComponents)
        comp->Initialize(
            ArgsForInit {device, cmdList, rItemsList, this}
        );
}

void Actor::Update(const GameTimer& gt)
{
    for (auto& comp : mComponents)
        comp->Update(
            ArgsForUpdate {gt.DeltaTime(), this}
        );
}
