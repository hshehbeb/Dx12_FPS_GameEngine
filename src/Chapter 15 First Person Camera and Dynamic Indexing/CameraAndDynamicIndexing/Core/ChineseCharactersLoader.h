#pragma once
#include "../../../Common/d3dUtil.h"
#include "../DataStructures/ChineseChar.h"
#include "Actors/Actor.h"

class ChineseCharactersLoader
{
public:
    void Load(int loadCount, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    /**
     * returns the idx of this char
     */
    int LoadCharacter(ChineseChar cnChar, ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList);
    
    Texture* GetByIndex(int idx);

private:
    typedef std::vector<std::unique_ptr<Texture>> t_TexturesList;

    t_TexturesList mCharTexList_Opaque;
    t_TexturesList mCharTexList_Transparent;
};
