#include "BatchBase.h"

#include "../../../Common/d3dApp.h"

void BatchBase::Add(std::shared_ptr<IBatchable> uiObj)
{
    mBatchables.Add(uiObj);
}

void BatchBase::InitAll(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    BuildPSOs(device);
    
    for (auto& uiObj : mBatchables.GetValues())
        uiObj->Initialize(device, cmdList);
}

void BatchBase::UpdateAll()
{
    for (auto& uiObj : mBatchables.GetValues())
        uiObj->Update();
}

void BatchBase::DrawAll(ID3D12GraphicsCommandList* cmdList)
{
    SetPSO(cmdList);
    SetRootSignature(cmdList);

    /*
     * why should mAxisIndicator being drawn before ?
     *
     * the rootSignature has 2 args, 1 for cbv and 1 for srv.
     * this class only fill the cbv and leaves the srv referencing
     * a stale place.
     * in pixel shader, we will sample the texture in srv according to uv,
     * likely we sampled an alpha value.
     * and cause this class simply not passing the ALPHA_TEST.
     */
    // mAxisIndicator->Draw(cmdList);
    
    for (auto& uiObj : mBatchables.GetValues())
        uiObj->Draw(cmdList);
}

void BatchBase::BuildPSO(
    ID3D12Device* device,
    IN std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
    IN ID3D12RootSignature* pRootSignature,
    IN D3D12_SHADER_BYTECODE vsByteCode,
    IN D3D12_SHADER_BYTECODE psByteCode,
    OUT Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO)

{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    
    psoDesc.InputLayout = {inputLayout.data(), static_cast<UINT>(inputLayout.size())};
    psoDesc.pRootSignature = pRootSignature;
    psoDesc.VS = vsByteCode;
    psoDesc.PS = psByteCode;
    
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = D3DApp::GetApp()->mBackBufferFormat;
    psoDesc.SampleDesc.Count = D3DApp::GetApp()->m4xMsaaState ? 4 : 1;
    psoDesc.SampleDesc.Quality = D3DApp::GetApp()->m4xMsaaState ? (D3DApp::GetApp()->m4xMsaaQuality - 1) : 0;
    psoDesc.DSVFormat = D3DApp::GetApp()->mDepthStencilFormat;

    ThrowIfFailed(device->CreateGraphicsPipelineState(
            &psoDesc, IID_PPV_ARGS(&resultPSO))
    );
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> BatchBase::GetStaticSamplers()
{
    // Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return { 
		pointWrap, pointClamp,
		linearWrap, linearClamp, 
		anisotropicWrap, anisotropicClamp };
}



