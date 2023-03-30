#include "ChineseCharactersLoader.h"
#include "../../../Common/DDSTextureLoader.h"

struct DDS_HEADER;

void ChineseCharactersLoader::Load(int loadCount, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    for (int i = 0; i < loadCount; i++)
    {
        auto charTex = std::make_unique<Texture>();
        charTex->Name = std::string("charTex") + std::to_string(i);
        charTex->Filename = L"../../Textures/uncompressed.dds";
        ThrowIfFailed(::CreateDDSTextureForNextChar(
            device, cmdList, charTex->Filename.c_str(),
            charTex->Resource, charTex->UploadHeap)
        );
        
        mCharTexList_Opaque.push_back(std::move(charTex));
    }
}

int ChineseCharactersLoader::LoadCharacter(ChineseChar cnChar, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    auto charTex = std::make_unique<Texture>();
    charTex->Name = std::string("charTex") + std::to_string( rand() );
    charTex->Filename = L"../../Textures/uncompressed.dds";
    ThrowIfFailed(::CreateDDSTextureForCnChar(
        cnChar, device, cmdList, charTex->Filename.c_str(),
        charTex->Resource, charTex->UploadHeap)
    );

    int idx = mCharTexList_Opaque.size();
    mCharTexList_Opaque.push_back(std::move(charTex));
    return idx;
}

Texture* ChineseCharactersLoader::GetByIndex(int idx)
{
    return mCharTexList_Opaque.at(idx).get();
}

