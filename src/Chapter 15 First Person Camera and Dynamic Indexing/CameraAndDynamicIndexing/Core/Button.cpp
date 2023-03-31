#include "Button.h"

#include "Log.h"
#include "UIWidgets/Image2D.h"

// void Button::Initialize(UIObjectsCollection* uiObjsCollection)
// {
//     uiObjsCollection->Add(mUIObject);
// }

Button::Button(ScreenSpacePoint& atPos, int width, int height, Texture* pTexture,
    std::function<void(ScreenSpacePoint)>&& onClickHandle, bool isVisible)
    : mPos(atPos)
    , mWidth(width)
    , mHeight(height)
    , mOnClickHandle(onClickHandle)
{
    image = std::make_shared<Image2D>(mPos, width, height, pTexture);
    SetShouldDraw(isVisible);
}

bool Button::CheckIfClicked(const ScreenSpacePoint& clickPos)
{
    if (!image->Visible)
        return false;
    
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
    image->Visible = newValue;
}

void Button::SetOnClickHandle(std::function<void(ScreenSpacePoint)> newHandle)
{
    mOnClickHandle = newHandle;
}

void Button::SetPosition(const ScreenSpacePoint& newPos)
{
    mPos = newPos;
    image->SetPosition(newPos);
}

const ScreenSpacePoint& Button::GetPosition() const
{
    return image->GetPosition();
}

