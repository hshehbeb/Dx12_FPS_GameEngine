#include "GravitySimulator.h"

#include "PlayerMovement.h"
#include "../Actors/Actor.h"

GravitySimulator::GravitySimulator(DirectX::XMFLOAT3 gravityDirection, float maximumSpeed)
    : GravityDirection(gravityDirection)
    , MaximumSpeed(maximumSpeed)
    , mCurrentVelocity(DirectX::XMFLOAT3 {0, 0, 0})
    , mGroundedY(2.0f)  // todo: eliminate this invariance
    , mIgnoreThisFrame(false)
{
}

void GravitySimulator::Update(ArgsForUpdate args)
{
    Actor* owner = args.Owner;
    
    if (mIgnoreThisFrame)
    {
        mIgnoreThisFrame = false;
        return;
    }
    
    PlayerMovement* comp;
    if (!owner->TryGetComponent(&comp)) return;
    
    if (IsGrounded(comp))
    {
        mCurrentVelocity = DirectX::XMFLOAT3 {};
        return;
    }

    /* update velocity */
    DirectX::XMFLOAT3 deltaVel =
    {
        GravityDirection.x * GRAVITY_FORCE,
        GravityDirection.y * GRAVITY_FORCE,
        GravityDirection.z * GRAVITY_FORCE
    };
    
    float sqrMagnitude
        = deltaVel.x * deltaVel.x + deltaVel.y * deltaVel.y + deltaVel.z * deltaVel.z; 

    if (sqrMagnitude <= MaximumSpeed * MaximumSpeed)
        mCurrentVelocity =
        {
            mCurrentVelocity.x + deltaVel.x,
            mCurrentVelocity.y + deltaVel.y,
            mCurrentVelocity.z + deltaVel.z 
        };

    /* apply velocity */
    DirectX::XMFLOAT3 translation =
    {
        mCurrentVelocity.x,
        mCurrentVelocity.y,
        mCurrentVelocity.z
    };
    comp->MoveFree(translation);
}

void GravitySimulator::Initialize(ArgsForInit args)
{
}

bool GravitySimulator::IsGrounded(PlayerMovement* comp)
{
    const float tolerance = 0.1f;
    return abs(comp->GetPosition().y - mGroundedY) < tolerance;
}

void GravitySimulator::IgnoreSimulationForThisFrame()
{
    mIgnoreThisFrame = true;
}
