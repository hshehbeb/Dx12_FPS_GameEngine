#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <wrl/client.h>

#include "../../../../Common/UploadBuffer.h"
#include "../../DataStructures/ScreenSpacePoint.h"
#include "../BatchProcess/IBatchable.h"

struct MeshGeometry;

class ImageBase : public IBatchable
{
    struct ConstantBuffer { DirectX::XMFLOAT4X4 MVPMatrix = MathHelper::Identity4x4(); };
    
public:
    bool shouldDraw;
    ScreenSpacePoint position;
    int width;
    int height;

public:
    explicit ImageBase(ScreenSpacePoint position, int width, int height, Texture* pTexture);

    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) override;
    void Draw(ID3D12GraphicsCommandList* cmdList) override;
    void Update() override;

private:
    std::unique_ptr<MeshGeometry> mQuadGeom;
    
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
    std::unique_ptr<UploadBuffer<ConstantBuffer>> mConstantBuffer = nullptr;
    Texture* mTexture = nullptr;

private:
    DirectX::XMFLOAT4X4 CalculateMVPMatrix() const;
    void BuildDescriptorHeap(ID3D12Device& device);
    void BuildConstantBuffer(ID3D12Device& device);
    void BuildQuadGeometry(ID3D12Device& device, ID3D12GraphicsCommandList& cmdList);
};
