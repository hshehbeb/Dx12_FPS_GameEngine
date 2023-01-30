#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <wrl/client.h>

#include "../../../Common/UploadBuffer.h"
#include "../DataStructures/ScreenSpacePoint.h"

struct MeshGeometry;

class UIObject
{
    struct ConstantBuffer { DirectX::XMFLOAT4X4 MVPMatrix = MathHelper::Identity4x4(); };
    
public:
    ScreenSpacePoint position;
    int width;
    int height;

public:
    UIObject(ScreenSpacePoint position, int width, int height);

    void Initialize(ID3D12Device& device, ID3D12GraphicsCommandList& cmdList);
    void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
    void Update();
    
    // DirectX::XMMATRIX GetWVPMatrix() { return mMVPMatrix; }
    Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() { return mRootSignature; }
    Microsoft::WRL::ComPtr<ID3DBlob> GetVertexShaderCode() { return mVSByteCode; }
    Microsoft::WRL::ComPtr<ID3DBlob> GetPixelShaderCode() { return mPSByteCode; }
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() { return mInputLayout; }

private:
    std::unique_ptr<MeshGeometry> mQuadGeom;
    
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
    std::unique_ptr<UploadBuffer<ConstantBuffer>> mConstantBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mVSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mPSByteCode = nullptr;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
    std::unique_ptr<Texture> mTexture = nullptr;

private:
    DirectX::XMFLOAT4X4 CalculateMVPMatrix() const;
    void LoadTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void BuildDescriptorHeap(ID3D12Device& device);
    void BuildConstantBuffer(ID3D12Device& device);
    void BuildRootSignature(ID3D12Device& device);
    void BuildShaderLayout();
    void BuildQuadGeometry(ID3D12Device& device, ID3D12GraphicsCommandList& cmdList);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};
