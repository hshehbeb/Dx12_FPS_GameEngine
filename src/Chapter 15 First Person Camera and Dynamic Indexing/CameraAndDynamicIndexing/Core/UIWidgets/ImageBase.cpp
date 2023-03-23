#include <D3Dcompiler.h>

#include "ImageBase.h"
#include "../../../../Common/DDSTextureLoader.h"
#include "../../../../Common/d3dApp.h"
#include "../../../../Common/d3dUtil.h"
#include "../../DataStructures/Vertex.h"

struct Vertex;

ImageBase::ImageBase(Texture* pTexture)
    : Visible(true)
    // , position(position)
    // , width(width)
    // , height(height)
    , mTexture(pTexture)
{
}

void ImageBase::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // LoadTexture(&device, &cmdList);
    BuildDescriptorHeap(*device);
    BuildConstantBuffer(*device);
    BuildQuadGeometry(device, cmdList);
}

void ImageBase::BuildDescriptorHeap(ID3D12Device& device)
{
    // constant buffer containing vertices info
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(device.CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&mCbvHeap)));


    // shader resource buffer containing tex
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(device.CreateDescriptorHeap(
        &srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap))
        );

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mTexture->Resource->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = mTexture->Resource->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    device.CreateShaderResourceView(mTexture->Resource.Get(), &srvDesc, hDescriptor);
}

void ImageBase::BuildConstantBuffer(ID3D12Device& device)
{
    mConstantBuffer = std::make_unique<UploadBuffer<ConstantBuffer>>(
        &device, 1, true);

    UINT cbByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ConstantBuffer));

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mConstantBuffer->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer
    int cbIndex = 0;
    cbAddress += cbIndex * cbByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ConstantBuffer));

    device.CreateConstantBufferView( /* we only need 1 cbv, for caching our matrix */
        &cbvDesc,
        mCbvHeap->GetCPUDescriptorHandleForHeapStart()
        );
}

void ImageBase::BuildQuadGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(DirectX::Colors::White);
    std::array<Vertex, 4> vertices =
    {
        Vertex{DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), color, DirectX::XMFLOAT2(0, 1)},
        Vertex{DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), color, DirectX::XMFLOAT2(0, 0)},
        Vertex{DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), color, DirectX::XMFLOAT2(1, 1)},
        Vertex{DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), color, DirectX::XMFLOAT2(1, 0)}
    };

    std::array<std::uint16_t, 6> indices =
    {
        0, 1, 2,
        1, 3, 2
    };

    const UINT vbByteSize = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
    const UINT ibByteSize = static_cast<UINT>(indices.size()) * sizeof(std::uint16_t);

    mQuadGeom = std::make_unique<MeshGeometry>();
    mQuadGeom->Name = "ui quad";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &mQuadGeom->VertexBufferCPU));
    CopyMemory(mQuadGeom->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &mQuadGeom->IndexBufferCPU));
    CopyMemory(mQuadGeom->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    mQuadGeom->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
                                                              cmdList, vertices.data(), vbByteSize,
                                                              mQuadGeom->VertexBufferUploader);

    mQuadGeom->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
                                                             cmdList, indices.data(), ibByteSize,
                                                             mQuadGeom->IndexBufferUploader);

    mQuadGeom->VertexByteStride = sizeof(Vertex);
    mQuadGeom->VertexBufferByteSize = vbByteSize;
    mQuadGeom->IndexFormat = DXGI_FORMAT_R16_UINT;
    mQuadGeom->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    mQuadGeom->DrawArgs["quad"] = submesh;
}

void ImageBase::Draw(ID3D12GraphicsCommandList* cmdList)
{
    if (!Visible)
    {
        return;
    }
    
    ID3D12DescriptorHeap* descriptorHeaps[] = {mSrvDescriptorHeap.Get()};
    cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    cmdList->SetGraphicsRootConstantBufferView(0, mConstantBuffer->Resource()->GetGPUVirtualAddress());
    cmdList->SetGraphicsRootDescriptorTable(1, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

    cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    cmdList->IASetVertexBuffers(0, 1, &mQuadGeom->VertexBufferView());
    cmdList->IASetIndexBuffer(&mQuadGeom->IndexBufferView());
    cmdList->DrawIndexedInstanced(
        mQuadGeom->DrawArgs["quad"].IndexCount,
        1, 0, 0, 0);
}

void ImageBase::Update()
{
    // Convert Spherical to Cartesian coordinates.
    // float x = mRadius*sinf(mPhi)*cosf(mTheta);
    // float z = mRadius*sinf(mPhi)*sinf(mTheta);
    // float y = mRadius*cosf(mPhi);

    // Build the view matrix.
    // XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    // XMVECTOR target = XMVectorZero();
    // XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //
    // XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    // ??? matrix
    // XMStoreFloat4x4(&mView, view);

    // XMMATRIX world = XMLoadFloat4x4(&mWorld);
    // XMMATRIX proj = XMLoadFloat4x4(&mProj);
    // XMMATRIX worldViewProj = world*view*proj;
    //
    // // Update the constant buffer with the latest worldViewProj matrix.
    // ObjectConstants objConstants;
    // XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    // mObjectCB->CopyData(0, objConstants);

    using namespace DirectX;

    XMFLOAT4X4 mvpMatrix = CalculateMVPMatrix();
    XMMATRIX transposed = XMMatrixTranspose(XMLoadFloat4x4(&mvpMatrix));
    XMStoreFloat4x4(&mvpMatrix, transposed);
    
    ConstantBuffer cb { mvpMatrix };
    // ConstantBuffer cb { MathHelper::Identity4x4() };
    mConstantBuffer->CopyData(0, cb);
}
