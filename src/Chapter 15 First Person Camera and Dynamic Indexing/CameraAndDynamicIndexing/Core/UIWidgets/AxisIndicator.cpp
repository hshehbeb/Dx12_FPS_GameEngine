#include "AxisIndicator.h"
#include "../../../../Common/d3dUtil.h"
#include "../../../../Common/d3dApp.h"
#include "../../DataStructures/Rotation.h"
#include "../../DataStructures/Vertex.h"

AxisIndicator::AxisIndicator(
    ScreenSpacePoint position, int width, int height, Camera* camera)
    : Position(position)
    , Width(width)
    , Height(height)
    , mCam(camera)
{
}

void AxisIndicator::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    BuildDescriptorHeap(*device, *cmdList);
    BuildActualDescriptor(*device);
    BuildGeometry(*device, *cmdList);
}

void AxisIndicator::Draw(ID3D12GraphicsCommandList* cmdList)
{
    cmdList->SetGraphicsRootConstantBufferView(
        0, mCb->Resource()->GetGPUVirtualAddress()
        );
    cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
    cmdList->IASetVertexBuffers(
        0, 1, &mGeom->VertexBufferView()
        );
    cmdList->IASetIndexBuffer(&mGeom->IndexBufferView());
    cmdList->DrawIndexedInstanced(
        mGeom->DrawArgs["axis"].IndexCount,
        1, 0,
        0, 0
        );
}

void AxisIndicator::Update()
{
    DirectX::XMFLOAT4X4 mvpMatrix = CalculateMVPMatrix();
    DirectX::XMMATRIX transposed = XMMatrixTranspose(XMLoadFloat4x4(&mvpMatrix));
    XMStoreFloat4x4(&mvpMatrix, transposed);
    
    ConstantBuffer cb { mvpMatrix };
    mCb->CopyData(0, cb);
}

void AxisIndicator::BuildDescriptorHeap(ID3D12Device& device, ID3D12GraphicsCommandList& cmd_list)
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(device.CreateDescriptorHeap(
        &cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap))
        );

    // D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    // srvHeapDesc.NumDescriptors = 1;
    // srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    // srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    // ThrowIfFailed(device.CreateDescriptorHeap(
    //     &srvHeapDesc, IID_PPV_ARGS(&mSrvHeap))
    //     );
}

void AxisIndicator::BuildActualDescriptor(ID3D12Device& device)
{
    mCb = std::make_unique<UploadBuffer<ConstantBuffer>>(
        &device, 1, true
        );
    UINT cbByteSize =
        d3dUtil::CalcConstantBufferByteSize(sizeof(ConstantBuffer));
    D3D12_GPU_VIRTUAL_ADDRESS cbAddress =
        mCb->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer
    int cbIndex = 0;
    cbAddress += cbIndex * cbByteSize;
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes =
        d3dUtil::CalcConstantBufferByteSize(sizeof(ConstantBuffer));
    
    /* we only need 1 cbv, for caching our matrix */
    device.CreateConstantBufferView(
        &cbvDesc, mCbvHeap->GetCPUDescriptorHandleForHeapStart()
        );
}

void AxisIndicator::BuildGeometry(ID3D12Device& device, ID3D12GraphicsCommandList& cmd_list)
{
    DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(DirectX::Colors::White);

    std::array<Vertex, 4> verts =
    {
        Vertex{ DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), color, DirectX::XMFLOAT2(0, 0) },
        Vertex{ DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), color, DirectX::XMFLOAT2(0, 0) },
        Vertex{ DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), color, DirectX::XMFLOAT2(0, 0) },
        Vertex{ DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), color, DirectX::XMFLOAT2(0, 0) }
    };

    std::array<std::uint16_t, 6> indices =
    {
        0, 1,
        0, 2,
        0, 3
    };

    
    mGeom = std::make_unique<MeshGeometry>();
    mGeom->Name = "axis";
    
    const UINT vbByteSize = static_cast<UINT>(verts.size()) * sizeof(Vertex);
    const UINT ibByteSize = static_cast<UINT>(indices.size()) * sizeof(std::uint16_t);
    ThrowIfFailed(D3DCreateBlob(vbByteSize, &mGeom->VertexBufferCPU));
    CopyMemory(mGeom->VertexBufferCPU->GetBufferPointer(), verts.data(), vbByteSize);
    ThrowIfFailed(D3DCreateBlob(ibByteSize, &mGeom->IndexBufferCPU));
    CopyMemory(mGeom->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    mGeom->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(
        &device, &cmd_list, verts.data(), vbByteSize, mGeom->VertexBufferUploader
        );
    mGeom->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(
        &device, &cmd_list, indices.data(), ibByteSize, mGeom->IndexBufferUploader
        );
    mGeom->VertexByteStride = sizeof(Vertex);
    mGeom->VertexBufferByteSize = vbByteSize;
    mGeom->IndexFormat = DXGI_FORMAT_R16_UINT;
    mGeom->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;
    mGeom->DrawArgs["axis"] = submesh;
}

DirectX::XMFLOAT4X4 AxisIndicator::CalculateMVPMatrix()
{
    float W = D3DApp::GetApp()->mClientWidth;
    float H = D3DApp::GetApp()->mClientHeight;
    
    float a = Width * 2 / W;
    float b = Position.x * 2 / W - 1;
    float c = Height * 2 / H;
    float d = -Position.y * 2 / H + 1;
    float z = 0.01f;

    auto moveMatr = DirectX::XMMATRIX
    {
        a, 0, 0, 0,
        0, c, 0, 0,
        0, 0, 0, 0,
        b, d, z, 1
    };

    Rotation rot = mCam->GetRotation(); 
    auto rotMatr = rot.CalculateMatrix();
    
    auto finalMatr = DirectX::XMMatrixMultiply(rotMatr, moveMatr);
    DirectX::XMFLOAT4X4 result {};
    DirectX::XMStoreFloat4x4(&result, finalMatr);
    
    return result;
}
