#pragma once
#include <DirectXMath.h>

#include "IComponent.h"

class AlwaysFaceTarget : public IComponent
{
public:
    AlwaysFaceTarget(Actor* target);
    
    void Initialize(ArgsForInit args) override;
    void Update(ArgsForUpdate args) override;

private:
    Actor* mTarget;
    
private:
    DirectX::XMFLOAT3 CalculateFaceAtRotation(Actor* src, Actor* dst);
    float EulerAngleY(DirectX::XMFLOAT3 vec1, DirectX::XMFLOAT3 vec2);
};
