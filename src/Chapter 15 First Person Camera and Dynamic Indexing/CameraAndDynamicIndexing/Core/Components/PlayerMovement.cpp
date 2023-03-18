#include "PlayerMovement.h"

#include "GravitySimulator.h"
#include "Transform.h"
#include "../Actors/Actor.h"

PlayerMovement::PlayerMovement(Camera& camera, float&& moveSpeed, float&& jumpSpeed)
    : MoveSpeed(moveSpeed)
    , JumpSpeed(jumpSpeed)
    , mMoveDirectionThisFrame(DirectX::XMFLOAT3 {0,0,0})
    , mCamera(camera)
{
}

void PlayerMovement::Update(Actor* owner)
{
    DirectX::XMFLOAT3 translation {};
    
    /* x, z axis */
    float magnitude = sqrtf(mMoveDirectionThisFrame.x * mMoveDirectionThisFrame.x + mMoveDirectionThisFrame.z * mMoveDirectionThisFrame.z);
    translation.x = (magnitude == 0) ?
        0 : mMoveDirectionThisFrame.x / magnitude * MoveSpeed;
    translation.z = (magnitude == 0) ?
        0 : mMoveDirectionThisFrame.z / magnitude * MoveSpeed;
    
    /* y axis */
    translation.y = (mMoveDirectionThisFrame.y == 0) ?
        0 : mMoveDirectionThisFrame.y * JumpSpeed;

    if (translation.x == 0 && translation.y == 0 && translation.z == 0)
        return;
    
    /* do the actual movement */
    DirectX::XMFLOAT3 beforeMv = GetPosition();
    DirectX::XMFLOAT3 afterMv  =
    {
        beforeMv.x + translation.x,
        beforeMv.y + translation.y,
        beforeMv.z + translation.z
    };
    mCamera.SetPosition(afterMv);

    Transform* trans;
    if (owner->TryGetComponent(&trans))
    {
        trans->SetPosition(afterMv);
    }

    /* ready for next frame to use */
    mMoveDirectionThisFrame = {};
}

void PlayerMovement::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList)
{
}

void PlayerMovement::MoveFwd()
{
    MoveClampToSurface(mCamera.GetLook3f());
}

void PlayerMovement::MoveBwd()
{
    MoveClampToSurface(ReverseVector(mCamera.GetLook3f()));
}

void PlayerMovement::MoveRwd()
{
    MoveClampToSurface(mCamera.GetRight3f());
}

void PlayerMovement::MoveLwd()
{
    MoveClampToSurface(ReverseVector(mCamera.GetRight3f()));
}

void PlayerMovement::Jump(Actor* owner)
{
    GravitySimulator* gravitySimulator;
    if (owner->TryGetComponent<GravitySimulator>(&gravitySimulator))
    {
        gravitySimulator->IgnoreSimulationForThisFrame();
    }
    
    DirectX::XMFLOAT3 mvDir = {0,1,0};
    MoveFree(mvDir);
}

void PlayerMovement::MoveFree(DirectX::XMFLOAT3 direction)
{
    mMoveDirectionThisFrame =
    {
        mMoveDirectionThisFrame.x + direction.x,
        mMoveDirectionThisFrame.y + direction.y,
        mMoveDirectionThisFrame.z + direction.z
    };
}

DirectX::XMFLOAT3 PlayerMovement::GetPosition()
{
    return mCamera.GetPosition3f();
}

void PlayerMovement::MoveClampToSurface(DirectX::XMFLOAT3 worldDir)
{
    DirectX::XMFLOAT3 clampedDir {worldDir.x, 0, worldDir.z};
    MoveFree(clampedDir);
}

DirectX::XMFLOAT3 PlayerMovement::ReverseVector(DirectX::XMFLOAT3 vec)
{
    return DirectX::XMFLOAT3 {-vec.x, -vec.y, -vec.z};
}
