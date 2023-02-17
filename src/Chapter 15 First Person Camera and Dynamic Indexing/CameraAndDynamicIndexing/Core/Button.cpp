#include "Button.h"

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

