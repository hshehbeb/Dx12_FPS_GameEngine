#include "AnythingBatch.h"

#include "../Utilis/UtilisFunctionLibery.h"

AnythingBatch::AnythingBatch(BatchArgs batchArgs, EfficientLookup<std::shared_ptr<IBatchable>> batchables)
    : mBatchables(std::move(batchables))
    , mBatchArgs(batchArgs)
{
}

void AnythingBatch::Add(std::shared_ptr<IBatchable> uiObj)
{
    mBatchables.Add(uiObj);
}

void AnythingBatch::InitAll(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    BuildRootSignature(device);
    BuildPSO(device);
    
    for (auto& uiObj : mBatchables.GetValues())
        uiObj->Initialize(device, cmdList);
}

void AnythingBatch::UpdateAll()
{
    for (auto& uiObj : mBatchables.GetValues())
        uiObj->Update();
}

void AnythingBatch::DrawAll(ID3D12GraphicsCommandList* cmdList)
{
    cmdList->SetGraphicsRootSignature(mRootSignature.Get());
    cmdList->SetPipelineState(mPSO.Get());

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

void AnythingBatch::BuildPSO(ID3D12Device* device)
{
    CompileShaders();

    D3D12_SHADER_BYTECODE vertShader =
    {
        reinterpret_cast<BYTE*>(mVertShader->GetBufferPointer()),
        mVertShader->GetBufferSize()
    };
    D3D12_SHADER_BYTECODE pixelShader =
    {
        reinterpret_cast<BYTE*>(mPxlShader->GetBufferPointer()),
        mPxlShader->GetBufferSize()
    };
    
    auto inputLayout = BuildInputLayout();
    Dx12Helpers::BuildPSO(
        device, inputLayout, mRootSignature.Get(),
        vertShader, pixelShader, mPSO
    );
}

std::vector<D3D12_INPUT_ELEMENT_DESC> AnythingBatch::BuildInputLayout()
{
    const D3D_SHADER_MACRO alphaTestDefines[] = {
        "ALPHA_TEST", "1",
        NULL, NULL
    };
        
    return {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
}

void AnythingBatch::CompileShaders()
{
    mVertShader = d3dUtil::CompileShader(mBatchArgs.PathToVertexShader,
        nullptr, "main", "vs_5_0");
    
    mPxlShader  = d3dUtil::CompileShader(mBatchArgs.PathToPixelShader,
        nullptr, "main", "ps_5_0");
}

void AnythingBatch::BuildRootSignature(ID3D12Device* device)
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

    ThrowIfFailed(device->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature))
        );
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> AnythingBatch::GetStaticSamplers()
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



