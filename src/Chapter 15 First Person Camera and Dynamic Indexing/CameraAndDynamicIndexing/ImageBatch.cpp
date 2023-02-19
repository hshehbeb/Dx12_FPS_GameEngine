#include "ImageBatch.h"

#include "../../Common/d3dApp.h"

void ImageBatch::Add(std::shared_ptr<Image> uiObj)
{
    mImages.Add(uiObj);
}

void ImageBatch::InitAll(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    for (auto& uiObj : mImages.GetValues())
        uiObj->Initialize(*device, *cmdList);

    mAxisIndicator->Initailize(*device, *cmdList);
}

void ImageBatch::UpdateAll()
{
    for (auto& uiObj : mImages.GetValues())
        uiObj->Update();

    mAxisIndicator->Update();
}

void ImageBatch::DrawAll(ID3D12GraphicsCommandList* cmdList)
{
    cmdList->SetPipelineState(mPSOs["std_ui"].Get());
    cmdList->SetGraphicsRootSignature(mRootSignatures["std_ui"].Get());
    
    for (auto& uiObj : mImages.GetValues())
        uiObj->Draw(cmdList);

    mAxisIndicator->Draw(cmdList);
}

ImageBatch::t_PSOsRegistry ImageBatch::BuildPSOs(ID3D12Device* device)
{
    t_PSOsRegistry result {};

    BuildStdUiPSO(device, result["std_ui"]);

    return result;
}

void ImageBatch::BuildPSO(
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

void ImageBatch::BuildStdUiPSO(
    IN ID3D12Device* device,
    OUT Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO)
    
{
    auto stdInputLayout = StdUiInputLayout();
    CompileStdShaders();
    BuildStdRootSignature(*device);

    D3D12_SHADER_BYTECODE vertShader =
    {
        reinterpret_cast<BYTE*>(mShaders["std_ui_VS"]->GetBufferPointer()),
        mShaders["std_ui_VS"]->GetBufferSize()
    };
    D3D12_SHADER_BYTECODE pixelShader =
    {
        reinterpret_cast<BYTE*>(mShaders["std_ui_PS"]->GetBufferPointer()),
        mShaders["std_ui_PS"]->GetBufferSize()
    };
    
    BuildPSO(
        device, stdInputLayout, mRootSignatures["std_ui"].Get(),
        vertShader, pixelShader, resultPSO
    );
}

std::vector<D3D12_INPUT_ELEMENT_DESC> ImageBatch::StdUiInputLayout()
{
    const D3D_SHADER_MACRO alphaTestDefines[] =
    {
        "ALPHA_TEST", "1",
        NULL, NULL
    };
    
    return {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
}

void ImageBatch::CompileStdShaders()
{
    mShaders["std_ui_VS"] = d3dUtil::CompileShader(L"Shaders\\vet_renderer2d.hlsl", nullptr, "main", "vs_5_0");
    mShaders["std_ui_PS"] = d3dUtil::CompileShader(L"Shaders\\pxl_renderer2d.hlsl", nullptr, "main", "ps_5_0");
}

void ImageBatch::BuildStdRootSignature(ID3D12Device& device)
{
    CD3DX12_ROOT_PARAMETER slotRootParameter[2];
    
    // Create a single descriptor table of CBVs.
    // CD3DX12_DESCRIPTOR_RANGE cbvTable;
    // cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    slotRootParameter[0].InitAsConstantBufferView(0);
    
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    slotRootParameter[1].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
    
    auto staticSamplers = GetStaticSamplers();
    
    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter,
        staticSamplers.size(), staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                             serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(device.CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignatures["std_ui"])));
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> ImageBatch::GetStaticSamplers()
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



