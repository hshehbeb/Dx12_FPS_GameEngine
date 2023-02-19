#pragma once
#include "Core/UIWidgets/AxisIndicator.h"
#include "Core/UIWidgets/Image.h"
#include "Resources.h"
#include "DataStructures/EfficientLookup.h"

class ImageBatch
{
public:
    ImageBatch(ID3D12Device* device, AxisIndicator* axisIndicator,
        EfficientLookup<std::shared_ptr<Image>>& images)
        : mImages(images)
    {
        mAxisIndicator = axisIndicator;
        
        mPSOs = BuildPSOs(device);
    }

    void Add(std::shared_ptr<Image> uiObj);
    
    void InitAll(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void UpdateAll();
    void DrawAll(ID3D12GraphicsCommandList* cmdList);

private:
    typedef EfficientLookup<std::shared_ptr<Image>> t_ImagesRegistry;
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> t_PSOsRegistry;
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> t_ShaderRegistry; 
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> t_RootSignatureRegistry; 

private:
    t_ImagesRegistry& mImages;
    t_PSOsRegistry mPSOs;
    t_ShaderRegistry mShaders;
    t_RootSignatureRegistry mRootSignatures;
    AxisIndicator* mAxisIndicator; 

private:
    t_PSOsRegistry BuildPSOs(ID3D12Device* device);
    void ImageBatch::BuildPSO(ID3D12Device* device, IN std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout, IN ID3D12RootSignature* pRootSignature, IN D3D12_SHADER_BYTECODE vsByteCode, IN D3D12_SHADER_BYTECODE psByteCode, OUT Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
    void BuildStdUiPSO(ID3D12Device* device, Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
    std::vector<D3D12_INPUT_ELEMENT_DESC> StdUiInputLayout();
    void CompileStdShaders();
    void BuildStdRootSignature(ID3D12Device& device);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};
