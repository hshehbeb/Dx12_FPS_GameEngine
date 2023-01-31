#include "Player.h"

Player::Player(Camera& camera, float speed, float jumpForce)
    : Speed(speed)
    , JumpForce(jumpForce)
    , mCamera(camera)
{
}

void Player::MoveFwd()
{
    Move(mCamera.GetLook3f());
}

void Player::MoveBwd()
{
    Move(ReverseVector(mCamera.GetLook3f()));
}

void Player::MoveRwd()
{
    Move(mCamera.GetRight3f());
}

void Player::MoveLwd()
{
    Move(ReverseVector(mCamera.GetRight3f()));
}

DirectX::XMFLOAT3 Player::GetPosition()
{
    return mCamera.GetPosition3f();
}

void Player::Move(DirectX::XMFLOAT3 worldDir)
{
    DirectX::XMFLOAT3 moveDir {worldDir.x, 0, worldDir.z};
    
    DirectX::XMFLOAT3 currPos = GetPosition(); 
    DirectX::XMFLOAT3 motion {moveDir.x * Speed, moveDir.y * Speed, moveDir.z * Speed};
    SetPosition({currPos.x + motion.x, currPos.y + motion.y, currPos.z + motion.z});
}

void Player::SetPosition(DirectX::XMFLOAT3 atPos)
{
    mCamera.SetPosition(atPos);
}

DirectX::XMFLOAT3 Player::ReverseVector(DirectX::XMFLOAT3 vec)
{
    return DirectX::XMFLOAT3 {-vec.x, -vec.y, -vec.z};
}


