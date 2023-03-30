#include "Image2D.h"
#include "../../../../Common/d3dApp.h"

Image2D::Image2D(ScreenSpacePoint screenPos, int width, int height, Texture* tex)
    : ImageBase(tex)
    , mScreenPos(screenPos)
    , mWidth(width)
    , mHeight(height) 
{
}

void Image2D::SetPosition(const ScreenSpacePoint& newPos)
{
    mScreenPos = newPos;
}

DirectX::XMFLOAT4X4 Image2D::CalculateMVPMatrix() const
{
    float W = D3DApp::GetApp()->mClientWidth;
    float H = D3DApp::GetApp()->mClientHeight;
    
    float a = mWidth * 2 / W;
    float b = mScreenPos.x * 2 / W - 1;
    float c = mHeight * 2 / H;
    float d = -mScreenPos.y * 2 / H + 1;
    float z = 0.01f;

    return DirectX::XMFLOAT4X4
    {
        a, 0, 0, 0,
        0, c, 0, 0,
        0, 0, 0, 0,
        b, d, z, 1
    };
}
