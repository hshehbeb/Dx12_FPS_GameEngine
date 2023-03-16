#include "ModelRenderer3D.h"

ModelRenderer3D::ModelRenderer3D(std::string loadPath, Material* material)
    : mPath(loadPath)
    , mMat(material)
{
}

// void ModelRenderer3D::RegisterRenderItem(std::vector<RenderItem>& renderItems,
//     ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
// {
//     LoadModel(device, cmdList);
// }

const std::string& ModelRenderer3D::GetModelName()
{
    return mPath;
}

void ModelRenderer3D::LoadModel(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    LoadFromDiskToMemory();
    LoadFromMemoryToGPU(device, cmdList);
}

void ModelRenderer3D::LoadFromDiskToMemory()
{
    std::unique_ptr<Model> model = std::make_unique<Model>(GetModelName(), mPath);
    
    XMMATRIX modelScale;
    float scale = 1;
    modelScale = XMMatrixScaling(scale, scale, scale);
    
    XMMATRIX modelRot;
    modelRot = XMMatrixRotationX(MathHelper::Pi / 2);// *XMMatrixRotationY(MathHelper::Pi / 2);
    
    XMMATRIX modelOffset;
    modelOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    
    model->SetScale(modelScale);
    model->SetRotate(modelRot);
    model->SetTransform(modelOffset);
    
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
        // item->ObjCBIndex = objCBIndex++;

        item->Mat = mMat.get();
        item->Obj = mGeoObject.get();
        item->Geo = mMeshGeo.get();
        item->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        item->NumFramesDirty = gNumFrameResources;
        item->IndexCount = mesh.IndexCount;
        item->StartIndexLocation = mesh.StartIndexLocation;
        item->BaseVertexLocation = mesh.BaseVertexLocation;
        // item->Bounds = item->Geo->DrawArgs[GetModelName()].Bounds;
        item->name = GetModelName() + std::to_string(rand());
        // n++;
        // mRitemsLayer[(int)layer].push_back(item.get());
        // mAllRitems.push_back(std::move(item));

        renderItems.push_back(std::move(item));
    }

    return renderItems;
}
