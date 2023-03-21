#pragma once

#include <d3d12.h>

class IBatchable
{
public:
    virtual void Initialize(ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList) = 0;

    virtual void Draw(ID3D12GraphicsCommandList* cmdList) = 0;

    virtual void Update() = 0;
};
