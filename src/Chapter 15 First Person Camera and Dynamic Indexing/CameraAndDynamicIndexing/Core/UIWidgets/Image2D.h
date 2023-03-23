#pragma once
#include "ImageBase.h"
#include "../../DataStructures/ScreenSpacePoint.h"

class Image2D : public ImageBase
{
public:
    Image2D(ScreenSpacePoint screenPos, int width, int height, Texture* tex);

protected:
    DirectX::XMFLOAT4X4 CalculateMVPMatrix() const override;

private:
    ScreenSpacePoint mScreenPos;
    int mWidth, mHeight;
};
