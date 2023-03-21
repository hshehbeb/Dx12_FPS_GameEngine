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

// BatchBase::t_PSOsRegistry BatchBase::BuildPSOs(ID3D12Device* device)
// {
//     t_PSOsRegistry result {};
//
//     BuildStdUiPSO(device, result["std_ui"]);
//     BuildStdOpaquePSO(device, result["std_opaque"]);
//
//     return result;
// }

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

void BatchBase::BuildStdOpaquePSO(
    IN ID3D12Device* device,
    OUT Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO)
    
{
    auto stdInputLayout = StdOpaqueInputLayout();
    CompileStdOpaqueShaders();
    BuildStdOpaqueRootSignature(*device);

    D3D12_SHADER_BYTECODE vertShader =
    {
        reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()), 
        mShaders["standardVS"]->GetBufferSize()
    };
    D3D12_SHADER_BYTECODE pixelShader =
    {
        reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
        mShaders["opaquePS"]->GetBufferSize()
    };
    
    BuildPSO(
        device, stdInputLayout, mRootSignatures["std_opaque"].Get(),
        vertShader, pixelShader, resultPSO
    );
}

std::vector<D3D12_INPUT_ELEMENT_DESC> BatchBase::StdOpaqueInputLayout()
{
    return {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
}

void BatchBase::CompileStdOpaqueShaders()
{
    mShaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");
}

void BatchBase::BuildStdOpaqueRootSignature(ID3D12Device& device)
{
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[4];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsConstantBufferView(0);
    slotRootParameter[1].InitAsConstantBufferView(1);
    slotRootParameter[2].InitAsShaderResourceView(0, 1);
    slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);


    auto staticSamplers = GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
        (UINT)staticSamplers.size(), staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if(errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(device.CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(mRootSignatures["std_opaque"].GetAddressOf()))
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



