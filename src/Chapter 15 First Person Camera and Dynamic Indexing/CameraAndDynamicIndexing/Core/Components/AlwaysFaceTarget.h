#pragma once
#include "IComponent.h"

class AlwaysFaceTarget : public IComponent
{
public:
    AlwaysFaceTarget(Actor* target);
    
    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
        RenderItemsList& rItemsList, Actor* owner) override;

    void Update(Actor* owner) override;

private:
    Actor* mTarget;
    
private:
    XMFLOAT3 CalculateFaceAtRotation(Actor* src, Actor* dst);
    float EulerAngleY(XMFLOAT3 vec1, XMFLOAT3 vec2);
};
