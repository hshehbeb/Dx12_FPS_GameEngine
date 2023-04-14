#pragma once
#include <d3d12.h>

class RenderItemsList;
class Actor;

struct ArgsForInit
{
    ID3D12Device* Device;
    ID3D12GraphicsCommandList* CmdList;
    RenderItemsList& RenderItems;
    Actor* Owner;
};
