#pragma once

#include "../../DataStructures/ArgsForInit.h"
#include "../../DataStructures/ArgsForUpdate.h"

class IComponent
{
public:
    virtual void Initialize(ArgsForInit args) = 0;
    virtual void Update(ArgsForUpdate args) = 0;
};
