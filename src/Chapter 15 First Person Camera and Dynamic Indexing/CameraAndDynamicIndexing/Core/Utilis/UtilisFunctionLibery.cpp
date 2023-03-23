#include "UtilisFunctionLibery.h"

float ActorUtilis::DistanceBetweenActors(Actor* actor1, Actor* actor2)
{
    Transform* trans1;
    if (!actor1->TryGetComponent(&trans1))
        throw std::invalid_argument("actor without a Transform is not locatable");

    Transform* trans2;
    if (!actor2->TryGetComponent(&trans2))
        throw std::invalid_argument("actor without a Transform is not locatable");;

    XMFLOAT3 pos1 = trans1->GetPosition(), pos2 = trans2->GetPosition();
    float distance = sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2) + pow(pos1.z - pos2.z, 2));
        
    return distance;
}

float MathHelpers::AngleOf2Vector2D(XMFLOAT2 vec1, XMFLOAT2 vec2)
{
    float dotProduct = DotProduct(vec1, vec2);
    float crossProductZ = vec1.x * vec2.y - vec1.y * vec2.x;
    float clockwiseSign = crossProductZ / abs(crossProductZ);
    
    float magVec1 = Magnitude(vec1);
    float magVec2 = Magnitude(vec2);

    return clockwiseSign * acos(dotProduct / (magVec1 * magVec2));
}

void ActorUtilis::ThrowRequireCompException(const char* compName)
{
    throw std::exception(compName);
}

void Dx12Helpers::BuildPSO(
    ID3D12Device* device,
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
    ID3D12RootSignature* pRootSignature,
    D3D12_SHADER_BYTECODE vsByteCode,
    D3D12_SHADER_BYTECODE psByteCode,
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

float MathHelpers::Magnitude(XMFLOAT2 vec2D)
{
    return sqrt(vec2D.x * vec2D.x + vec2D.y * vec2D.y); 
}

float MathHelpers::DotProduct(XMFLOAT2 vec1, XMFLOAT2 vec2)
{
    return vec1.x * vec2.x + vec1.y * vec2.y;
}

XMFLOAT2 MathHelpers::Minus(XMFLOAT2 vec1, XMFLOAT2 vec2)
{
    return XMFLOAT2 {vec1.x - vec2.x, vec1.y - vec2.y};
}

XMFLOAT3 MathHelpers::Minus(XMFLOAT3 vec1, XMFLOAT3 vec2)
{
    return XMFLOAT3 {vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z};
}
