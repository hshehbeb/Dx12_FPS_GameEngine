#include "Image3D.h"

Image3D::Image3D(float width, float height, Transform* trans, Texture* tex, Camera* cam)
    : ImageBase(tex)
    , mWidth(width)
    , mHeight(height)
    , mTrans(trans)
    , mCamera(cam)
{
}

XMFLOAT4X4 Image3D::CalculateMVPMatrix() const
{
    XMFLOAT4X4 mvpMatr;
    XMStoreFloat4x4(
        &mvpMatr,
        mTrans->GetWorldMatrix() * mCamera->GetView() * mCamera->GetProj()
        );
    
    return mvpMatr;
}