#pragma once
#include "BatchBase.h"

class StdUIBatch : public BatchBase
{
public:
    StdUIBatch(EfficientLookup<std::shared_ptr<IBatchable>> batchables = {});
    
protected:
    void SetRootSignature(ID3D12GraphicsCommandList* cmdList) override;
    void SetPSO(ID3D12GraphicsCommandList* cmdList) override;
    void BuildPSOs(ID3D12Device* device) override;
    
private:
    void BuildStdUiPSO(ID3D12Device* device, Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
    std::vector<D3D12_INPUT_ELEMENT_DESC> StdUiInputLayout();
    void CompileStdShaders();
    void BuildStdRootSignature(ID3D12Device& device);
};
