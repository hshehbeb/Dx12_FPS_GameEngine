#pragma once
#include "ImageBase.h"
#include "../../DataStructures/ScreenSpacePoint.h"

class Image2D : public ImageBase
{
public:
    Image2D(ScreenSpacePoint screenPos, int width, int height, Texture* tex);

    void SetPosition(const ScreenSpacePoint& newPos);

protected:
    DirectX::XMFLOAT4X4 CalculateMVPMatrix() const override;

private:
    ScreenSpacePoint mScreenPos;
    int mWidth, mHeight;
};
