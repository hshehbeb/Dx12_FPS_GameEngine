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
    bool CheckIfClicked(const ScreenSpacePoint& clickPos);
    virtual void HandleOnClick();

    // todo: should pull up
    ScreenSpacePoint GetMinCorner();
    ScreenSpacePoint GetMaxCorner();

private:
    ScreenSpacePoint mPos;
    int mWidth, mHeight;
    std::shared_ptr<UIObject> mUIObject;
};
