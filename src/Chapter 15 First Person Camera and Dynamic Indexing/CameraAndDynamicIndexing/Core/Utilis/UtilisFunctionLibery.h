#pragma once

#include "../../../Common/d3dApp.h"
#include "../Actors/Actor.h"
#include "../Components/Transform.h"

namespace ActorUtilis
{
    float DistanceBetweenActors(Actor* actor1, Actor* actor2);
    // XMFLOAT3 FaceAtRotation(Actor* src, Actor* dst);
    void ThrowRequireCompException(const char* compName);
}

namespace MathHelpers
{
    float AngleOf2Vector2D(XMFLOAT2 vec1, XMFLOAT2 vec2);
    // XMFLOAT3 EulerAngleOf2Vector3D(XMFLOAT3 vec1, XMFLOAT3 vec2);
    float Magnitude(XMFLOAT2 vec2D);
    float DotProduct(XMFLOAT2 vec1, XMFLOAT2 vec2);
    // XMFLOAT2 CrossProduct(XMFLOAT2 vec1, XMFLOAT2 vec2);
    XMFLOAT2 Minus(XMFLOAT2 vec1, XMFLOAT2 vec2);
    XMFLOAT3 Minus(XMFLOAT3 vec1, XMFLOAT3 vec2);
}

namespace Dx12Helpers
{
    void BuildPSO(
        ID3D12Device* device,
        IN std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
        IN ID3D12RootSignature* pRootSignature,
        IN D3D12_SHADER_BYTECODE vsByteCode,
        IN D3D12_SHADER_BYTECODE psByteCode,
        OUT Microsoft::WRL::ComPtr<ID3D12PipelineState>& resultPSO);
}
