#pragma once
#include <DirectXMath.h>

#include "IComponent.h"
#include "../../../../Common/Camera.h"

class PlayerMovement : public IComponent
{
public:
    float MoveSpeed;
    float JumpSpeed;
    
public:
    PlayerMovement(Camera& camera, float&& moveSpeed = 0.005f, float&& jumpSpeed = 0.005f);
    
    virtual void Update(Actor* owner) override;
    virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList) override;

    void MoveFwd();
    void MoveBwd();
    void MoveRwd();
    void MoveLwd();
    void Jump(Actor* owner);
    void MoveFree(DirectX::XMFLOAT3 direction);

    DirectX::XMFLOAT3 GetPosition();

private:
    DirectX::XMFLOAT3 mMoveDirectionThisFrame;
    Camera& mCamera;

private:
    void MoveClampToSurface(DirectX::XMFLOAT3 worldDir);
    DirectX::XMFLOAT3 ReverseVector(DirectX::XMFLOAT3 vec);
};
