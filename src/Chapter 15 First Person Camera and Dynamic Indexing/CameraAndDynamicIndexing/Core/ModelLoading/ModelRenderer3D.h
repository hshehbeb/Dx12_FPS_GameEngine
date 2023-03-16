#pragma once
#include <memory>

#include "Model.h"
#include "../../DataStructures/RenderItem.h"

class ModelRenderer3D
{
public:
    ModelRenderer3D(std::string loadPath, Material* material);

    std::vector<std::unique_ptr<RenderItem>> BuildRenderItems();
    void LoadModel(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

private:
    const std::string& GetModelName();
    void LoadFromDiskToMemory();
    void LoadFromMemoryToGPU(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

private:
    std::string mPath;
    std::shared_ptr<Material> mMat;
    std::unique_ptr<Model> mModel;
    std::unique_ptr<GeoObject> mGeoObject;
    std::unique_ptr<MeshGeometry> mMeshGeo;
};
