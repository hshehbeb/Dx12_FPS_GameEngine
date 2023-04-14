#include "ImageRender3D.h"

ImageRender3D::ImageRender3D(AnythingBatch* batch, float width, float height, Texture* tex, Camera* cam)
    : mWidth(width)
    , mHeight(height)
    , mTexture(tex)
    , mProjCamera(cam)
    , mBatch(batch)
{
}

void ImageRender3D::Update(ArgsForUpdate args)
{
}

void ImageRender3D::Initialize(ArgsForInit args)
{
    Actor* owner = args.Owner;
    
    Transform* trans;
    if (!owner->TryGetComponent(&trans))
    {
        throw std::exception("missing comp");
    }
    
    mImage = std::make_shared<Image3D>(mWidth, mHeight, trans, mTexture, mProjCamera);
    mBatch->Add(mImage);
}
