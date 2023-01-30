#include <D3Dcompiler.h>

#include "UIObject.h"

#include "../../../Common/d3dApp.h"
#include "../../../Common/d3dUtil.h"
#include "../DataStructures/Vertex.h"

struct Vertex;

UIObject::UIObject(ScreenSpacePoint position, int width, int height)
    : position(position)
    , width(width)
    , height(height)
{
}

void UIObject::Initialize(ID3D12Device& device, ID3D12GraphicsCommandList& cmdList)
{
    LoadTexture(&device, &cmdList);
    BuildRootSignature(device);
    BuildDescriptorHeap(device);
    BuildConstantBuffer(device);
    BuildShaderLayout();
    BuildQuadGeometry(device, cmdList);
}

DirectX::XMFLOAT4X4 UIObject::CalculateMVPMatrix() const
{
    float W = D3DApp::GetApp()->mClientWidth;
    float H = D3DApp::GetApp()->mClientHeight;
    
    float a = width / W;
    float b = position.x / W - 1;
    float c = height / H;
    float d = position.y / H - 1;
    float z = 0.01f;

    return DirectX::XMFLOAT4X4
    {
        a, 0, 0, 0,
        0, c, 0, 0,
        0, 0, 0, 0,
        b, d, z, 1
    };
}

void UIObject::LoadTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    mTexture = std::make_unique<Texture>();
    mTexture->Name = "uiTex";
    mTexture->Filename = L"../../Textures/WoodCrate01.dds";
    
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(device,
        cmdList, mTexture->Filename.c_str(),
        mTexture->Resource, mTexture->UploadHeap)
    );
}

void UIObject::BuildDescriptorHeap(ID3D12Device& device)
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

void UIObject::BuildConstantBuffer(ID3D12Device& device)
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
        mCbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void UIObject::BuildRootSignature(ID3D12Device& device)
{
    CD3DX12_ROOT_PARAMETER slotRootParameter[2];
    
    // Create a single descriptor table of CBVs.
    // CD3DX12_DESCRIPTOR_RANGE cbvTable;
    // cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    slotRootParameter[0].InitAsConstantBufferView(0);
    
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    slotRootParameter[1].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
    
    auto staticSamplers = GetStaticSamplers();
    
    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter,
        staticSamplers.size(), staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                             serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(device.CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature)));
}

void UIObject::BuildShaderLayout()
{
    const D3D_SHADER_MACRO alphaTestDefines[] =
    {
        "ALPHA_TEST", "1",
        NULL, NULL
    };

    mVSByteCode = d3dUtil::CompileShader(L"Shaders\\vet_renderer2d.hlsl", nullptr, "main", "vs_5_0");
    mPSByteCode = d3dUtil::CompileShader(L"Shaders\\pxl_renderer2d.hlsl", nullptr, "main", "ps_5_0");

    mInputLayout =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
}

void UIObject::BuildQuadGeometry(ID3D12Device& device, ID3D12GraphicsCommandList& cmdList)
{
    // const UINT mbByteSize = sizeof mWVPMatrix;
    //
    // ThrowIfFailed(D3DCreateBlob(mbByteSize, &mWVPMatBufferCPU));
    // CopyMemory(mWVPMatBufferCPU->GetBufferPointer(), &mWVPMatrix, mbByteSize);
    //
    // mWVPMatBufferGPU = d3dUtil::CreateDefaultBuffer(&device, &cmdList, &mWVPMatrix,
    //     mbByteSize, mWVPMatBufferUploader);


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

    mQuadGeom->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(&device,
                                                              &cmdList, vertices.data(), vbByteSize,
                                                              mQuadGeom->VertexBufferUploader);

    mQuadGeom->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(&device,
                                                             &cmdList, indices.data(), ibByteSize,
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

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> UIObject::GetStaticSamplers()
{
    	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return { 
		pointWrap, pointClamp,
		linearWrap, linearClamp, 
		anisotropicWrap, anisotropicClamp };
}

void UIObject::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList)
{
    ID3D12DescriptorHeap* descriptorHeaps[] = {mSrvDescriptorHeap.Get()};
    cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    cmdList->SetGraphicsRootSignature(mRootSignature.Get());
    cmdList->SetGraphicsRootConstantBufferView(0, mConstantBuffer->Resource()->GetGPUVirtualAddress());
    cmdList->SetGraphicsRootDescriptorTable(1, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

    cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    cmdList->IASetVertexBuffers(0, 1, &mQuadGeom->VertexBufferView());
    cmdList->IASetIndexBuffer(&mQuadGeom->IndexBufferView());
    cmdList->DrawIndexedInstanced(
        mQuadGeom->DrawArgs["quad"].IndexCount,
        1, 0, 0, 0);
}

void UIObject::Update()
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
