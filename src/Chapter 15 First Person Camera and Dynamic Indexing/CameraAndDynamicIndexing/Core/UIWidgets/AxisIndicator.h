#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl/client.h>

#include "../../../../Common/Camera.h"
#include "../../../../Common/UploadBuffer.h"
#include "../../DataStructures/ScreenSpacePoint.h"

class AxisIndicator
{
    struct ConstantBuffer { DirectX::XMFLOAT4X4 MVPMatrix = MathHelper::Identity4x4(); };

public:
    ScreenSpacePoint Position;
    int Width;
    int Height;
    
public:
    AxisIndicator(ScreenSpacePoint position, int width, int height, Camera* camera);
    
    void Initailize(ID3D12Device& device, ID3D12GraphicsCommandList& cmdList);
    void Draw(ID3D12GraphicsCommandList* cmdList);
    void Update();

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap; 
    // Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
    std::unique_ptr<UploadBuffer<ConstantBuffer>> mCb;
    std::unique_ptr<MeshGeometry> mGeom;
    Camera* mCam;

private:
    void BuildDescriptorHeap(ID3D12Device& device, ID3D12GraphicsCommandList& cmd_list);
    void BuildActualDescriptor(ID3D12Device& device);
    void BuildGeometry(ID3D12Device& device, ID3D12GraphicsCommandList& cmd_list);
    DirectX::XMFLOAT4X4 CalculateMVPMatrix();
};
