#include "Image3D.h"

Image3D::Image3D(int width, int height, Transform* trans, Texture* tex)
    : ImageBase(tex)
    , mWidth(width)
    , mHeight(height)
    , mTrans(trans)
{
}

XMFLOAT4X4 Image3D::CalculateMVPMatrix() const
{
    mTrans->SetScale(XMFLOAT3 {100, 100, 100});
    
    auto worldMatrix = mTrans->GetWorldMatrix();
    
    XMFLOAT4X4 result;
    XMStoreFloat4x4(&result, worldMatrix);
    
    return result;
}
