#pragma once

#include "IBatchable.h"
#include "../Core/UIWidgets/AxisIndicator.h"
#include "../Core/UIWidgets/ImageBase.h"
#include "../Resources.h"
#include "../DataStructures/EfficientLookup.h"

/**
 * represents a batch of renderables
 * a batch aims to group similar renderables together as a single DrawCall
 */
class AnythingBatch
{
public:
    struct BatchArgs
    {
    public:
        std::wstring PathToVertexShader;
        std::wstring PathToPixelShader;
    };
    
public:
    AnythingBatch(BatchArgs batchArgs,
        EfficientLookup<std::shared_ptr<IBatchable>> batchables = {});

    void Add(std::shared_ptr<IBatchable> uiObj);
    void AddRange(const std::vector<std::shared_ptr<IBatchable>>& objs);

    void InitAll(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void UpdateAll();
    void DrawAll(ID3D12GraphicsCommandList* cmdList);

private:
    typedef EfficientLookup<std::shared_ptr<IBatchable>> t_BatchablesRegistry;

    t_BatchablesRegistry mBatchables;
    BatchArgs mBatchArgs;
    Microsoft::WRL::ComPtr<ID3DBlob> mVertShader;
    Microsoft::WRL::ComPtr<ID3DBlob> mPxlShader;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

private:
    void BuildPSO(ID3D12Device* device);
    std::vector<D3D12_INPUT_ELEMENT_DESC> BuildInputLayout();
    void CompileShaders();
    void BuildRootSignature(ID3D12Device* device);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};
