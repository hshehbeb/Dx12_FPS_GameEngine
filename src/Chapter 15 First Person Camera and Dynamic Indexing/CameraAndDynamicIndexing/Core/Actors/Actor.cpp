#include "Actor.h"

Actor::Actor(std::vector<std::shared_ptr<IComponent>>&& components)
{
    mComponents = std::move(components);
}

void Actor::Update()
{
    for (auto& comp : mComponents)
    {
        comp->Update(this);
    }
}
