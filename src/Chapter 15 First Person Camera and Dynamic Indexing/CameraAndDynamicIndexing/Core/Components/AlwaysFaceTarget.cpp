#include "AlwaysFaceTarget.h"

#include "Transform.h"
#include "../Actors/Actor.h"
#include "../Utilis/UtilisFunctionLibery.h"

AlwaysFaceTarget::AlwaysFaceTarget(Actor* target)
    : mTarget(target)
{
}

void AlwaysFaceTarget::Initialize(ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList, Actor* owner)
{
}

void AlwaysFaceTarget::Update(Actor* owner)
{
    Transform* ownerTrans;
    if (!owner->TryGetComponent(&ownerTrans))
        ActorUtilis::ThrowRequireCompException(typeid(Transform).name());

    ownerTrans->SetRotation(
        CalculateFaceAtRotation(owner, mTarget)
        );
}

XMFLOAT3 AlwaysFaceTarget::CalculateFaceAtRotation(Actor* src, Actor* dst)
{
    Transform* srcTrans;
    if (!src->TryGetComponent(&srcTrans))
        ActorUtilis::ThrowRequireCompException(typeid(Transform).name());
    
    Transform* dstTrans;
    if (!dst->TryGetComponent(&dstTrans))
        ActorUtilis::ThrowRequireCompException(typeid(Transform).name());

    XMFLOAT3 srcPos = srcTrans->GetPosition();
    XMFLOAT3 dstPos = dstTrans->GetPosition();
    
    XMFLOAT3 faceDir = MathHelpers::Minus(dstPos, srcPos);
    XMFLOAT3 dftFaceDir = XMFLOAT3 {0, 0, -1};

    return {0, EulerAngleY(faceDir, dftFaceDir), 0}; 
}

float AlwaysFaceTarget::EulerAngleY(XMFLOAT3 vec1, XMFLOAT3 vec2)
{
    // float angleX = AngleOf2Vector2D(
    //     XMFLOAT2 {vec1.z, vec1.y},
    //     XMFLOAT2 {vec2.z, vec2.y}
    // );
    // angleX = (std::isnan(angleX) ? 0 : angleX);

    float angleY = -MathHelpers::AngleOf2Vector2D(
        XMFLOAT2 {vec1.z, vec1.x},
        XMFLOAT2 {vec2.z, vec2.x}
    );
    angleY = (std::isnan(angleY) ? 0 : angleY);

    // float angleZ = AngleOf2Vector2D(
    //     XMFLOAT2 {vec1.x, vec1.y},
    //     XMFLOAT2 {vec2.x, vec2.y}
    // );
    // angleZ = (std::isnan(angleZ) ? 0 : angleZ);

    return angleY;
}
