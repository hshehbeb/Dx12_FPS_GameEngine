#pragma once

#include "../../../../Common/Camera.h"
#include "../Core/Components/Transform.h"
#include "../UIWidgets/ImageBase.h"
#include "../Components/IComponent.h"

/**
 * a component that rely on Transform to indicate
 * where to draw a given Image in the scene
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
