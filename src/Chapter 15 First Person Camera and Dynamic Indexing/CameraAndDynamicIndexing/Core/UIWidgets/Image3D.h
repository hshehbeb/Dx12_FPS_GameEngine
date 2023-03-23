#pragma once

#include "../../../../Common/Camera.h"
#include "../Core/Components/Transform.h"
#include "../UIWidgets/ImageBase.h"
#include "../Components/IComponent.h"

/**
 * aims to render an image within the world
 */
class Image3D : public ImageBase
{
public:
    Image3D(float width, float height, Transform* trans, Texture* tex, Camera* cam);
    
protected:
    XMFLOAT4X4 CalculateMVPMatrix() const override;

private:
    float mWidth, mHeight;
    Transform* mTrans;
    Camera* mCamera;
};
