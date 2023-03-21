#pragma once
#include "IBatchable.h"
#include "../Core/UIWidgets/AxisIndicator.h"
#include "../Core/UIWidgets/ImageBase.h"
#include "../Resources.h"
#include "../DataStructures/EfficientLookup.h"

/**
 * represent a batch
 * which will group a bunch of IBatchables together into one DrawCall
 */
class AnythingBatch
{
public:
    AnythingBatch(ID3D12Device* device,
                  EfficientLookup<std::shared_ptr<IBatchable>> batchables = {})
        : mBatchables(std::move(batchables))
    {
        mPSOs = BuildPSOs(device);
    }

    void Add(std::shared_ptr<IBatchable> uiObj);
    
    void InitAll(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void UpdateAll();
    void DrawAll(ID3D12GraphicsCommandList* cmdList);

private:
    typedef EfficientLookup<std::shared_ptr<IBatchable>> t_BatchablesRegistry;
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> t_PSOsRegistry;
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> t_ShaderRegistry; 
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> t_RootSignatureRegistry; 

private:
    t_BatchablesRegistry mBatchables;
    t_PSOsRegistry mPSOs;
    t_ShaderRegistry mShaders;
    t_RootSignatureRegistry mRootSignatures;

private:
    t_PSOsRegistry BuildPSOs(ID3D12Device* device);
    void AnythingBatch::BuildPSO(ID3D12Device* device, IN std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout, IN ID3D12RootSignature* pRootSignature, IN D3D12_SHADER_BYTECODE vsByteCode, IN D3D12_SHADER_BYTECODE psByteCode, OUT Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
    void BuildStdUiPSO(ID3D12Device* device, Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
    std::vector<D3D12_INPUT_ELEMENT_DESC> StdUiInputLayout();
    void CompileStdShaders();
    void BuildStdRootSignature(ID3D12Device& device);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};
