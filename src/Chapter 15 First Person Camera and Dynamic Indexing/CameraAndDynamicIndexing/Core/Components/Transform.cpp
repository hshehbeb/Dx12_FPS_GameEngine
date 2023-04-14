#include "Transform.h"

Transform::Transform(XMFLOAT3 pos, XMFLOAT3 scale, XMFLOAT3 rot)
{
    SetPosition(pos);
    SetRotation(rot);
    SetScale(scale);
}

void Transform::Initialize(ArgsForInit)
{
}

void Transform::Update(ArgsForUpdate)
{
}

void Transform::SetPosition(XMFLOAT3 newPos)
{
    mPosition_f3   = newPos;
    mPosition_Matr = XMMatrixTranslation(newPos.x, newPos.y, newPos.z);
}

void Transform::SetRotation(XMFLOAT3 newEulerAngle)
{
    mRotation_f3   = newEulerAngle;
    mRotation_Matr = XMMatrixRotationRollPitchYaw(newEulerAngle.x, newEulerAngle.y, newEulerAngle.z);
}

void Transform::SetScale(XMFLOAT3 newScale)
{
    mScale_f3   = newScale;
    mScale_Matr = XMMatrixScaling(newScale.x, newScale.y, newScale.z);
}

XMMATRIX Transform::GetWorldMatrix() const
{
    return mScale_Matr * mRotation_Matr * mPosition_Matr;
}

XMFLOAT3 Transform::GetPosition()
{
    return mPosition_f3;
}

XMFLOAT3 Transform::GetRotation()
{
    return mRotation_f3;
}

XMFLOAT3 Transform::GetScale()
{
    return mScale_f3;
}
