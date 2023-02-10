#pragma once
#include <DirectXMath.h>
#include "IComponent.h"
#include "PlayerMovement.h"

class Player;

class GravitySimulator : public IComponent
{
public:
    const float GRAVITY_FORCE = 0.00098f;
    
public:
    DirectX::XMFLOAT3 GravityDirection;
    float MaximumSpeed;

public:
    GravitySimulator(DirectX::XMFLOAT3 gravityDirection = DirectX::XMFLOAT3 {0, -1, 0}, float maximumSpeed = 1.0f);
    
    virtual void Update(Actor* owner) override;
    bool IsGrounded(PlayerMovement* comp);
    void IgnoreSimulationForThisFrame();

private:
    DirectX::XMFLOAT3 mCurrentVelocity;
    float mGroundedY;
    bool mIgnoreThisFrame;
};