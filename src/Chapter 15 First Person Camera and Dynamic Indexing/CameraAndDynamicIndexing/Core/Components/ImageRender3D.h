﻿#pragma once
#include "IComponent.h"
#include "../Actors/Actor.h"
#include "../BatchProcess/AnythingBatch.h"
#include "../UIWidgets/Image3D.h"

/**
 * a simple wrapper class for component-lize Image3D
 *
 *   in future version, this class could be refactored into
 *   a generic anything renderer, which hides the detail of
 *   managing batching operations
 */
class ImageRender3D : public IComponent
{
public:
    ImageRender3D(AnythingBatch* batch, float width, float height,
        Texture* tex, Camera* cam);
    
    void Initialize(ArgsForInit args) override;
    void Update(ArgsForUpdate args) override;

private:
    float mWidth, mHeight;
    Texture* mTexture;
    Camera* mProjCamera;
    AnythingBatch* mBatch;
    std::shared_ptr<Image3D> mImage;
};
