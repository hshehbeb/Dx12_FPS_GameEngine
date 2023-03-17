#pragma once
#include <d3d12.h>

#include "../../DataStructures/RenderItemsList.h"

struct RenderItem;
class Actor;

class IComponent
{
public:
    virtual void Initialize(ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList) = 0;
    
    virtual void Update(Actor* owner) = 0;
};
