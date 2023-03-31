#pragma once
#include <functional>
#include <memory>

#include "../DataStructures/ScreenSpacePoint.h"
#include "UIWidgets/Image2D.h"

struct Texture;

class Button
{
public:
    std::shared_ptr<Image2D> image;
    
public:
    Button(ScreenSpacePoint& atPos, int width, int height, Texture* pTexture,
           std::function<void(ScreenSpacePoint)>&& onClickHandle, bool isVisible = true);
    
    // void Initialize(UIObjectsCollection* uiObjsCollection);
    bool CheckIfClicked(const ScreenSpacePoint& clickPos);
    virtual void HandleOnClick(ScreenSpacePoint clickPos);

    // todo: should pull up
    ScreenSpacePoint GetMinCorner();
    ScreenSpacePoint GetMaxCorner();
    void SetShouldDraw(bool newValue);
    void SetOnClickHandle(std::function<void(ScreenSpacePoint)> newHandle);
    void SetPosition(const ScreenSpacePoint& newPos);
    const ScreenSpacePoint& GetPosition() const;

private:
    ScreenSpacePoint mPos;
    int mWidth, mHeight;
    std::function<void (ScreenSpacePoint)> mOnClickHandle;
};