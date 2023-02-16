#pragma once
#include "Core/UIWidgets/AxisIndicator.h"
#include "Core/UIWidgets/UIObject.h"
#include "DataStructures/ScreenSpacePoint.h"
#include "Resources.h"

// namespace Resources
// {
//     const unsigned int AvailableCharactersCount;
//     std::vector<std::unique_ptr<Texture>> CharacterTextures;
// }

class UIObjectsCollection
{
public:
    UIObjectsCollection(ID3D12Device* device, AxisIndicator* axisIndicator)
    {
        mAxisIndicator = axisIndicator;
        
        mUIObjectsRegistry["sample_text"] = std::make_unique<UIObject>(
            ScreenSpacePoint {200, 200},
            100, 100,
            Resources::CharacterTextures[0].get()
            );
        mUIObjectsRegistry["sample_text2"] = std::make_unique<UIObject>(
            ScreenSpacePoint {300, 300},
            50, 50,
            Resources::CharacterTextures[1].get()
            );

        /* player body */
        mUIObjectsRegistry["player_body"] = std::make_unique<UIObject>(
            ScreenSpacePoint {1200, 200},
            550, 500,
            Resources::RegularTextures["playerTex"].get()
            );

        /* crosshairs */
        mUIObjectsRegistry["crosshairs"] = std::make_unique<UIObject>(
            ScreenSpacePoint {800, 620},
            70, 70,
            Resources::RegularTextures["crosshairsTex"].get()
            );
        
        mUIObjects = GrabAllUIObjectsFromRegistry();
        mPSOs = BuildPSOs(device);
    }
    
    void InitAll(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void UpdateAll();
    void DrawAll(ID3D12GraphicsCommandList* cmdList);

private:
    typedef std::unordered_map<std::string, std::shared_ptr<UIObject>> t_UIObjectsRegistry;
    typedef std::vector<std::shared_ptr<UIObject>> t_UIObjectsList;
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> t_PSOsRegistry;
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> t_ShaderRegistry; 
    typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> t_RootSignatureRegistry; 

private:
    t_UIObjectsRegistry mUIObjectsRegistry;
    t_UIObjectsList mUIObjects;
    t_PSOsRegistry mPSOs;
    t_ShaderRegistry mShaders;
    t_RootSignatureRegistry mRootSignatures;
    AxisIndicator* mAxisIndicator; 

private:
    t_UIObjectsList GrabAllUIObjectsFromRegistry() const;
    t_PSOsRegistry BuildPSOs(ID3D12Device* device);
    void UIObjectsCollection::BuildPSO(ID3D12Device* device, IN std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout, IN ID3D12RootSignature* pRootSignature, IN D3D12_SHADER_BYTECODE vsByteCode, IN D3D12_SHADER_BYTECODE psByteCode, OUT Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
    void BuildStdUiPSO(ID3D12Device* device, Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
    std::vector<D3D12_INPUT_ELEMENT_DESC> StdUiInputLayout();
    void CompileStdShaders();
    void BuildStdRootSignature(ID3D12Device& device);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};
