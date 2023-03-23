#pragma once

#include "../../../Common/d3dApp.h"
#include "../Actors/Actor.h"
#include "../Components/Transform.h"

namespace ActorUtilis
{
    float DistanceBetweenActors(Actor* actor1, Actor* actor2);
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
