#include "Button.h"

#include "Log.h"

Button::Button(ScreenSpacePoint& atPos, int width, int height, Texture* pTexture)
    : mPos(atPos)
    , mWidth(width)
    , mHeight(height)
{
    mUIObject = std::make_shared<UIObject>(mPos, width, height, pTexture);
}

void Button::Initialize(UIObjectsCollection* uiObjsCollection)
{
    uiObjsCollection->Add(mUIObject);
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

void Button::HandleOnClick()
{
    Log::Debug("detected a click, further behaviours should be specified in child class");
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

