#pragma once
#include <memory>

#include "../ImageBatch.h"
#include "../DataStructures/ScreenSpacePoint.h"
#include "UIWidgets/Image.h"

struct Texture;

class Button
{
public:
    std::shared_ptr<Image> image;
    
public:
    explicit Button(ScreenSpacePoint& atPos, int width, int height, Texture* pTexture);

    // void Initialize(UIObjectsCollection* uiObjsCollection);
    bool CheckIfClicked(const ScreenSpacePoint& clickPos);
    virtual void HandleOnClick();

    // todo: should pull up
    ScreenSpacePoint GetMinCorner();
    ScreenSpacePoint GetMaxCorner();
    void SetShouldDraw(bool newValue);

private:
    ScreenSpacePoint mPos;
    int mWidth, mHeight;
};
