#pragma once
#include <DirectXCollision.h>

#include "../../../Common/Camera.h"

XM_DEPRECATED class Player
{
public:
    float Speed;
    float JumpForce;
    
public:
    Player(Camera& camera, float speed = 0.005f, float jumpForce = 5);
    ~Player() = default;
    Player(const Player& other) = delete;
    
    void MoveFwd();
    void MoveBwd();
    void MoveRwd();
    void MoveLwd();
    void Jump();
    
    DirectX::XMFLOAT3 GetPosition();

private:
    Camera& mCamera;

private:
    void Move(DirectX::XMFLOAT3 worldDir);
    void SetPosition(DirectX::XMFLOAT3 atPos);
    DirectX::XMFLOAT3 ReverseVector(DirectX::XMFLOAT3 vec);
};
