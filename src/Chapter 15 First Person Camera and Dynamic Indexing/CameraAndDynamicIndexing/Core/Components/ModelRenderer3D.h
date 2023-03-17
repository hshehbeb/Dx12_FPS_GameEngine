#pragma once
#include <memory>

#include "../ModelLoading/Model.h"
#include "../../DataStructures/RenderItem.h"
#include "../Components/IComponent.h"


class ModelRenderer3D : public IComponent
{
public:
    ModelRenderer3D(std::string loadPath, Material* material);
    ~ModelRenderer3D() = default;

    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList) override;
    void Update(Actor* owner) override;

private:
    std::vector<std::unique_ptr<RenderItem>> BuildRenderItems();
    void LoadModel(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    const std::string& GetModelName();
    void LoadFromDiskToMemory();
    void LoadFromMemoryToGPU(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

private:
    std::string mPath;
    Material* mMat;
    std::unique_ptr<GeoObject> mGeoObject;
    std::unique_ptr<MeshGeometry> mMeshGeo;
    std::vector<RenderItem*> mRenderItems;
};
