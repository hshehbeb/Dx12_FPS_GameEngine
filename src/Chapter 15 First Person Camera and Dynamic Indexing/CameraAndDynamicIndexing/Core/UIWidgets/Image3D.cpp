#include "Image3D.h"

Image3D::Image3D(float width, float height, Transform* trans, Texture* tex, Camera* cam)
    : mWidth(width)
    , mHeight(height)
    , mTrans(trans)
    , mTexture(tex)
    , mCamera(cam)
{
}

void Image3D::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    BuildDescriptorHeaps(device, cmdList);
    BuildConstantBuffer(device);
    // BuildRootSignature(device);
    BuildQuadGeometry(device, cmdList);
}

void Image3D::Draw(ID3D12GraphicsCommandList* cmdList)
{
    ID3D12DescriptorHeap* descriptorHeaps[] = {mSrvDescriptorHeap.Get()};
    cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    cmdList->SetGraphicsRootConstantBufferView(0, mConstantBuffer->Resource()->GetGPUVirtualAddress());
    cmdList->SetGraphicsRootDescriptorTable(1, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

    cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    cmdList->IASetVertexBuffers(0, 1, &mGeoObject->VertexBufferView());
    cmdList->IASetIndexBuffer(&mGeoObject->IndexBufferView());
    cmdList->DrawIndexedInstanced(
        mGeoObject->DrawArgs["quad3D"].IndexCount,
        1, 0, 0, 0);
}

void Image3D::Update()
{
    // XMFLOAT4X4 mvpMatrix = CalculateMVPMatrix();
    // XMMATRIX transposed = XMMatrixTranspose(XMLoadFloat4x4(&mvpMatrix));
    // XMStoreFloat4x4(&mvpMatrix, transposed);
    //
    // ConstantBuffer cb { mvpMatrix };
    // mConstantBuffer->CopyData(0, cb);

    XMFLOAT4X4 mvpMatrix;
    XMStoreFloat4x4(&mvpMatrix, mTrans->GetWorldMatrix() * mCamera->GetView() * mCamera->GetProj());
    XMMATRIX transposed = XMMatrixTranspose(XMLoadFloat4x4(&mvpMatrix));
    XMStoreFloat4x4(&mvpMatrix, transposed);
    ConstantBuffer cb { mvpMatrix };
    mConstantBuffer->CopyData(0, cb);
}

void Image3D::BuildQuadGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    GeometryGenerator geoGen {};
    GeometryGenerator::MeshData quad
        = geoGen.CreateQuad(0, mHeight, mWidth, mHeight, 0);

    SubmeshGeometry quadSubmesh;
    quadSubmesh.IndexCount = (UINT)quad.Indices32.size();
    quadSubmesh.StartIndexLocation = 0;
    quadSubmesh.BaseVertexLocation = 0;

    auto totalVerticesCount = quad.Vertices.size();

    std::vector<Vertex> vertices(totalVerticesCount);
    for (size_t i = 0; i < quad.Vertices.size(); ++i)
    {
        vertices[i].Pos   = quad.Vertices[i].Position;
        vertices[i].Color = XMFLOAT4 {1, 1, 1, 1};
        vertices[i].Uv    = quad.Vertices[i].TexC;
    }

    std::vector<std::uint16_t> indices;
    indices.insert(indices.end(), quad.GetIndices16().begin(), quad.GetIndices16().end());

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size()  * sizeof(std::uint16_t);

    mGeoObject = std::make_unique<MeshGeometry>();
    mGeoObject->Name = "quad3DGeo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &mGeoObject->VertexBufferCPU));
    CopyMemory(mGeoObject->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &mGeoObject->IndexBufferCPU));
    CopyMemory(mGeoObject->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    mGeoObject->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
        cmdList, vertices.data(), vbByteSize, mGeoObject->VertexBufferUploader);

    mGeoObject->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
        cmdList, indices.data(), ibByteSize, mGeoObject->IndexBufferUploader);

    mGeoObject->VertexByteStride = sizeof(Vertex);
    mGeoObject->VertexBufferByteSize = vbByteSize;
    mGeoObject->IndexFormat = DXGI_FORMAT_R16_UINT;
    mGeoObject->IndexBufferByteSize = ibByteSize;

    mGeoObject->DrawArgs["quad3D"] = quadSubmesh;
}

// void Image3D::BuildRootSignature(ID3D12Device* device)
// {
//     CD3DX12_ROOT_PARAMETER slotRootParameter[2];
//     
//     // Create a single descriptor table of CBVs.
//     // CD3DX12_DESCRIPTOR_RANGE cbvTable;
//     // cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
//     slotRootParameter[0].InitAsConstantBufferView(0);
//     
//     CD3DX12_DESCRIPTOR_RANGE texTable;
//     texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
//     slotRootParameter[1].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
//     
//     auto staticSamplers = GetStaticSamplers();
//     
//     // A root signature is an array of root parameters.
//     CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter,
//         staticSamplers.size(), staticSamplers.data(),
//         D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
//         );
//
//     // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
//     Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
//     Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
//     HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
//                                              serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
//
//     if (errorBlob != nullptr)
//     {
//         ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
//     }
//     ThrowIfFailed(hr);
//
//     ThrowIfFailed(device->CreateRootSignature(
//         0,
//         serializedRootSig->GetBufferPointer(),
//         serializedRootSig->GetBufferSize(),
//         IID_PPV_ARGS(&mRootSignature))
//         );
// }

void Image3D::BuildDescriptorHeaps(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // constant buffer containing vertices info
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&mCbvHeap)));


    // shader resource buffer containing tex
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(device->CreateDescriptorHeap(
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
    device->CreateShaderResourceView(mTexture->Resource.Get(), &srvDesc, hDescriptor);
}

void Image3D::BuildConstantBuffer(ID3D12Device* device)
{
    mConstantBuffer = std::make_unique<UploadBuffer<ConstantBuffer>>(
        device, 1, true);

    UINT cbByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ConstantBuffer));

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mConstantBuffer->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer
    int cbIndex = 0;
    cbAddress += cbIndex * cbByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ConstantBuffer));

    device->CreateConstantBufferView( /* we only need 1 cbv, for caching our matrix */
        &cbvDesc,
        mCbvHeap->GetCPUDescriptorHandleForHeapStart()
        );
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> Image3D::GetStaticSamplers()
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

// XMFLOAT4X4 Image3D::CalculateMVPMatrix() const
// {
//     mTrans->SetScale(XMFLOAT3 {100, 100, 100});
//     
//     auto worldMatrix = mTrans->GetWorldMatrix();
//     
//     XMFLOAT4X4 result;
//     XMStoreFloat4x4(&result, worldMatrix);
//     
//     return result;
// }
