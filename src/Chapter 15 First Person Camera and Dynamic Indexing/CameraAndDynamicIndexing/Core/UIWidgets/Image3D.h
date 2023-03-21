#pragma once

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
    Image3D(int width, int height, Transform* trans, Texture* tex);

protected:
    XMFLOAT4X4 CalculateMVPMatrix() const override;
    
private:
    int mWidth, mHeight;
    Transform* mTrans;
};
