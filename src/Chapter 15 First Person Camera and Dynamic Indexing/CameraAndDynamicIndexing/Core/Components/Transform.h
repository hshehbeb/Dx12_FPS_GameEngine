#pragma once
#include "IComponent.h"

class Transform : public IComponent
{
public:
    Transform(
        XMFLOAT3 pos = {0, 0, 0},
        XMFLOAT3 scale = {1, 1, 1},
        XMFLOAT3 rot = {MathHelper::Pi / 2, 0, 0}
    );

    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList) override;
    void Update(Actor* owner) override;

    void SetPosition(XMFLOAT3 newPos);
    void SetRotation(XMFLOAT3 newEulerAngle);
    void SetScale(XMFLOAT3 newScale);

    XMMATRIX GetWorldMatrix() const;
    XMFLOAT3 GetPosition();
    XMFLOAT3 GetRotation();
    XMFLOAT3 GetScale();

private:
    XMFLOAT3 mPosition_f3;
    XMFLOAT3 mRotation_f3;
    XMFLOAT3 mScale_f3;
    XMMATRIX mPosition_Matr;
    XMMATRIX mRotation_Matr;
    XMMATRIX mScale_Matr;
};
