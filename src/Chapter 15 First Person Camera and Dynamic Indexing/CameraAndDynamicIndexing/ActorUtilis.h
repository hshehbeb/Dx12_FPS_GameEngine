#pragma once
#include "Core/Actors/Actor.h"
#include "Core/Components/Transform.h"

namespace ActorUtilis
{
    float DistanceBetweenActors(Actor* actor1, Actor* actor2)
    {
        Transform* trans1;
        if (!actor1->TryGetComponent(&trans1))
            throw std::invalid_argument("actor without a Transform is not locatable");

        Transform* trans2;
        if (!actor2->TryGetComponent(&trans2))
            throw std::invalid_argument("actor without a Transform is not locatable");;

        XMFLOAT3 pos1 = trans1->GetPosition(), pos2 = trans2->GetPosition();
        float distance = sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2) + pow(pos1.z - pos2.z, 2));
        
        return distance;
    }
}
