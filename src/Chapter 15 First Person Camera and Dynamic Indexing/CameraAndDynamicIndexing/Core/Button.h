#pragma once
#include <functional>
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
    Button(ScreenSpacePoint& atPos, int width, int height, Texture* pTexture,
           std::function<void(ScreenSpacePoint)>&& onClickHandle);
    
    // void Initialize(UIObjectsCollection* uiObjsCollection);
    bool CheckIfClicked(const ScreenSpacePoint& clickPos);
    virtual void HandleOnClick(ScreenSpacePoint clickPos);

    // todo: should pull up
    ScreenSpacePoint GetMinCorner();
    ScreenSpacePoint GetMaxCorner();
    void SetShouldDraw(bool newValue);

private:
    ScreenSpacePoint mPos;
    int mWidth, mHeight;
    std::function<void (ScreenSpacePoint)> mOnClickHandle;
};