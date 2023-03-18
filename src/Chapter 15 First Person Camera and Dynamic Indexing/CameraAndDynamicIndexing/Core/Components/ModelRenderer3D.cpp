#include "ModelRenderer3D.h"

#include "Transform.h"
#include "../Actors/Actor.h"

ModelRenderer3D::ModelRenderer3D(std::string loadPath, Material* material)
    : mPath(loadPath)
    , mMat(material)
    , mRenderItems(std::vector<RenderItem*>())
{
}

void ModelRenderer3D::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList)
{
    LoadModel(device, cmdList);

    for (auto& renderItem : BuildRenderItems())
    {
        mRenderItems.push_back(renderItem.get());
        rItemsList.RegisterRenderItem(renderItem);
    }
}

const std::string& ModelRenderer3D::GetModelName()
{
    return mPath;
}

void ModelRenderer3D::LoadModel(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    LoadFromDiskToMemory();
    LoadFromMemoryToGPU(device, cmdList);
}

void ModelRenderer3D::Update(Actor* owner)
{
    Transform* transform;
    if (owner->TryGetComponent(&transform))
    {
        for (auto& renderItem : mRenderItems)
            XMStoreFloat4x4(
                &renderItem->World,
                transform->GetWorldMatrix()
            );
    }
}

void ModelRenderer3D::LoadFromDiskToMemory()
{
    auto model = std::make_unique<Model>(GetModelName(), mPath);

    XMMATRIX dftScale = XMMatrixScaling(1, 1, 1);
    model->SetScale(dftScale);

    XMMATRIX dftRot = XMMatrixRotationX(MathHelper::Pi / 2);
    model->SetRotate(dftRot);

    XMMATRIX dftPos = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    model->SetTransform(dftPos);
    
    mGeoObject = std::move(model);
}

void ModelRenderer3D::LoadFromMemoryToGPU(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    auto indices = mGeoObject->GetIndices32();
    auto vertices = mGeoObject->Vertices;

    UINT vbByteSize = mGeoObject->GetVertexCount() * sizeof(Vertex);
    UINT ibByteSize = indices.size() * sizeof(std::uint32_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = GetModelName();

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
    CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
        cmdList, vertices.data(), vbByteSize, geo->VertexBufferUploader);
    geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
        cmdList, indices.data(), ibByteSize, geo->IndexBufferUploader);

    geo->VertexByteStride = sizeof(Vertex);
    geo->VertexBufferByteSize = vbByteSize;
    geo->IndexFormat = DXGI_FORMAT_R32_UINT;
    geo->IndexBufferByteSize = ibByteSize;

    mMeshGeo = std::move(geo);
}

std::vector<std::unique_ptr<RenderItem>> ModelRenderer3D::BuildRenderItems()
{
	std::vector<std::unique_ptr<RenderItem>> renderItems {};
    
    for (int i = 0; i < mGeoObject->Meshes.size(); i++) {
        auto item = std::make_unique<RenderItem>();
        auto mesh = mGeoObject->Meshes[i];

        XMStoreFloat4x4(&item->World, mGeoObject->GetWorld());
        item->TexTransform = mesh.material.MatTransform;

        item->Mat = mMat;
        item->Obj = mGeoObject.get();
        item->Geo = mMeshGeo.get();
        item->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        item->NumFramesDirty = gNumFrameResources;
        item->IndexCount = mesh.IndexCount;
        item->StartIndexLocation = mesh.StartIndexLocation;
        item->BaseVertexLocation = mesh.BaseVertexLocation;
        item->name = GetModelName() + std::to_string(rand());

        renderItems.push_back(std::move(item));
    }

    return renderItems;
}
