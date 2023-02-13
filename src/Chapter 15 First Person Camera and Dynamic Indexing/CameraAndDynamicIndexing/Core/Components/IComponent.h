#pragma once

class Actor;

class IComponent
{
public:
    virtual void Update(Actor* owner) = 0;
};
