#include "Button.h"

#include "Log.h"

// void Button::Initialize(UIObjectsCollection* uiObjsCollection)
// {
//     uiObjsCollection->Add(mUIObject);
// }

Button::Button(ScreenSpacePoint& atPos, int width, int height, Texture* pTexture,
    std::function<void(ScreenSpacePoint)>&& onClickHandle)
    : mPos(atPos)
    , mWidth(width)
    , mHeight(height)
    , mOnClickHandle(onClickHandle)
{
    image = std::make_shared<ImageBase>(mPos, width, height, pTexture);
}

bool Button::CheckIfClicked(const ScreenSpacePoint& clickPos)
{
    auto maxCorner = GetMaxCorner();
    auto minCorner = GetMinCorner();
    
    return clickPos.x >= minCorner.x
           && clickPos.x <= maxCorner.x
           && clickPos.y >= minCorner.y
           && clickPos.y <= maxCorner.y;
}

void Button::HandleOnClick(ScreenSpacePoint clickPos)
{
    mOnClickHandle(clickPos);
}

ScreenSpacePoint Button::GetMinCorner()
{
    return ScreenSpacePoint {
        mPos.x - mWidth / 2,
        mPos.y - mHeight / 2
    };
}

ScreenSpacePoint Button::GetMaxCorner()
{
    return ScreenSpacePoint {
        mPos.x + mWidth / 2,
        mPos.y + mHeight / 2
    };
}

void Button::SetShouldDraw(bool newValue)
{
    image->shouldDraw = newValue;
}

