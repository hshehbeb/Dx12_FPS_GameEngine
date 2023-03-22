#pragma once

#include "IBatchable.h"
#include "../Core/UIWidgets/AxisIndicator.h"
#include "../Core/UIWidgets/ImageBase.h"
#include "../Resources.h"
#include "../DataStructures/EfficientLookup.h"

/**
 * represent a base class of all kinds of batches
 * a `batch` groups a bunch of IBatchables together into one DrawCall
 */
class BatchBase
{
public:
    explicit BatchBase(EfficientLookup<std::shared_ptr<IBatchable>> batchables = {})
        : mBatchables(std::move(batchables))
    {
    }

    void Add(std::shared_ptr<IBatchable> uiObj);
    
    void InitAll(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void UpdateAll();
    void DrawAll(ID3D12GraphicsCommandList* cmdList);

protected:
    typedef EfficientLookup<std::shared_ptr<IBatchable>> t_BatchablesRegistry;
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> t_PSOsRegistry;
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> t_ShaderRegistry; 
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> t_RootSignatureRegistry; 

protected:
    t_BatchablesRegistry mBatchables;
    t_PSOsRegistry mPSOs;
    t_ShaderRegistry mShaders;
    t_RootSignatureRegistry mRootSignatures;

protected:
    virtual void SetPSO(ID3D12GraphicsCommandList* cmdList) = 0;
    virtual void SetRootSignature(ID3D12GraphicsCommandList* cmdList) = 0;
    virtual void BuildPSOs(ID3D12Device* device) = 0;
    
    void BatchBase::BuildPSO(ID3D12Device* device, IN std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout, IN ID3D12RootSignature* pRootSignature, IN D3D12_SHADER_BYTECODE vsByteCode, IN D3D12_SHADER_BYTECODE psByteCode, OUT Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};
