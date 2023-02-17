#pragma once
#include <memory>

#include "../UIObjectsCollection.h"
#include "../DataStructures/ScreenSpacePoint.h"
#include "UIWidgets/UIObject.h"

struct Texture;

class Button
{
public:
    explicit Button(ScreenSpacePoint& atPos, int width, int height, Texture* pTexture);

    void Initialize(UIObjectsCollection* uiObjsCollection);

private:
    ScreenSpacePoint mPos;
    int mWidth, mHeight;
    std::shared_ptr<UIObject> mUIObject;
};
