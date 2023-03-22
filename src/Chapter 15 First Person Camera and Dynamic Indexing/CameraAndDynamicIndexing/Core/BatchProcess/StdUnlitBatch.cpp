#include "StdUnlitBatch.h"

StdUnlitBatch::StdUnlitBatch(EfficientLookup<std::shared_ptr<IBatchable>> batchables)
    : BatchBase(std::move(batchables))
{
}

void StdUnlitBatch::SetRootSignature(ID3D12GraphicsCommandList* cmdList)
{
    cmdList->SetGraphicsRootSignature(mRootSignatures["std_unlit"].Get());
}

void StdUnlitBatch::SetPSO(ID3D12GraphicsCommandList* cmdList)
{
    cmdList->SetPipelineState(mPSOs["std_unlit"].Get());
}

void StdUnlitBatch::BuildPSOs(ID3D12Device* device)
{
    BuildStdUnlitPSO(device, mPSOs["std_unlit"]);
}

void StdUnlitBatch::BuildStdUnlitPSO(ID3D12Device* device, Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO)
{
    auto stdInputLayout = BuildInputLayout();
    CompileShaders();
    BuildRootSignature(device);

    D3D12_SHADER_BYTECODE vertShader =
    {
        reinterpret_cast<BYTE*>(mShaders["std_unlit_VS"]->GetBufferPointer()), 
        mShaders["std_unlit_VS"]->GetBufferSize()
    };
    D3D12_SHADER_BYTECODE pixelShader =
    {
        reinterpret_cast<BYTE*>(mShaders["std_unlit_PS"]->GetBufferPointer()),
        mShaders["std_unlit_PS"]->GetBufferSize()
    };
    
    BuildPSO(
        device, stdInputLayout, mRootSignatures["std_unlit"].Get(),
        vertShader, pixelShader, resultPSO
    );
}

std::vector<D3D12_INPUT_ELEMENT_DESC> StdUnlitBatch::BuildInputLayout()
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

void StdUnlitBatch::CompileShaders()
{
    mShaders["std_unlit_VS"] = d3dUtil::CompileShader(L"Shaders\\vet_stdUnlit.hlsl", nullptr, "main", "vs_5_0");
    mShaders["std_unlit_PS"] = d3dUtil::CompileShader(L"Shaders\\pxl_stdUnlit.hlsl", nullptr, "main", "ps_5_0");
}

void StdUnlitBatch::BuildRootSignature(ID3D12Device* device)
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
        IID_PPV_ARGS(&mRootSignatures["std_unlit"]))
        );
}
