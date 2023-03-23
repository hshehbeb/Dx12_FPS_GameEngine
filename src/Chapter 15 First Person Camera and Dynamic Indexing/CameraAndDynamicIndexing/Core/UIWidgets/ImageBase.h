#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl/client.h>

#include "../../../../Common/UploadBuffer.h"
#include "../BatchProcess/IBatchable.h"

struct MeshGeometry;

class ImageBase : public IBatchable
{
    struct ConstantBuffer { DirectX::XMFLOAT4X4 MVPMatrix = MathHelper::Identity4x4(); };
    
public:
    
    /**
     * whether this Image should be drawn
     */
    bool Visible;

public:
    explicit ImageBase(Texture* pTexture);

    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) override;
    void Draw(ID3D12GraphicsCommandList* cmdList) override;
    void Update() override;

protected:
    std::unique_ptr<MeshGeometry> mQuadGeom;
    
private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
    std::unique_ptr<UploadBuffer<ConstantBuffer>> mConstantBuffer = nullptr;
    Texture* mTexture = nullptr;

protected:
    virtual DirectX::XMFLOAT4X4 CalculateMVPMatrix() const = 0;

private:
    void BuildDescriptorHeap(ID3D12Device& device);
    void BuildConstantBuffer(ID3D12Device& device);
    void BuildQuadGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};
