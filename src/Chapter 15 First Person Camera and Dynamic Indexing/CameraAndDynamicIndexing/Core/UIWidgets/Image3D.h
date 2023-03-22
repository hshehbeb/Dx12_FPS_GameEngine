#pragma once

#include "../../../../Common/Camera.h"
#include "../Core/Components/Transform.h"
#include "../UIWidgets/ImageBase.h"
#include "../Components/IComponent.h"

/**
 * a component that rely on Transform to indicate
 * where to draw a given Image in the scene
 */
class Image3D : public IBatchable
{
    struct ConstantBuffer { XMFLOAT4X4 MVPMatrix = MathHelper::Identity4x4(); };
    struct Vertex { XMFLOAT3 Pos; XMFLOAT4 Color; XMFLOAT2 Uv; };

public:
    Image3D(float width, float height, Transform* trans, Texture* tex, Camera* cam);

    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) override;
    void Draw(ID3D12GraphicsCommandList* cmdList) override;
    void Update() override;

// protected:
//     XMFLOAT4X4 CalculateMVPMatrix() const override;
    
private:
    float mWidth, mHeight;
    Transform* mTrans;
    Texture* mTexture;
    Camera* mCamera;
    std::unique_ptr<MeshGeometry> mGeoObject; 
    
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
    std::unique_ptr<UploadBuffer<ConstantBuffer>> mConstantBuffer = nullptr;

private:
    void BuildQuadGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void BuildDescriptorHeaps(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void BuildConstantBuffer(ID3D12Device* device);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> Image3D::GetStaticSamplers();
};
