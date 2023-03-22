#pragma once
#include "BatchBase.h"

class StdUnlitBatch : public BatchBase
{
public:
    StdUnlitBatch(EfficientLookup<std::shared_ptr<IBatchable>> batchables = {});

protected:
    void SetRootSignature(ID3D12GraphicsCommandList* cmdList) override;
    void SetPSO(ID3D12GraphicsCommandList* cmdList) override;
    void BuildPSOs(ID3D12Device* device) override;

private:
    void BuildStdUnlitPSO(ID3D12Device* device, Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
    std::vector<D3D12_INPUT_ELEMENT_DESC> BuildInputLayout();
    void CompileShaders();
    void BuildRootSignature(ID3D12Device* device);
};
