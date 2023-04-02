#pragma once
#include "../../Common/d3dUtil.h"
#include "Core/Button.h"
#include "Core/ChineseCharactersLoader.h"


class Scripter;

namespace Resources
{
    typedef std::unordered_map<std::string, std::unique_ptr<Texture>> t_TextureLookup;
    
    extern t_TextureLookup RegularTextures;
    extern ChineseCharactersLoader CnCharLoader;
    extern std::shared_ptr<Scripter> gScripter;
    extern std::vector<std::shared_ptr<Button>> gChoiceButtons;
    
/**
 * to use a handy LoadTexture macro to eliminate writing boilerplate,
 * you will have to follow following rules:
 * 1. put texture file under "../../Textures/" folder
 * 2. name the texture as "T_{#id}.dss"
 *    where {#id} should be substituted by id that you input as parameter
 */
#define LoadTexture(id) \
    LoadTextureByPath(id, std::wstring(L"../../Textures/T_") + std::wstring(L#id) + std::wstring(L".dds"))

/**
 * filePath should be std::wstring family
 */
#define LoadTextureByPath(id, filePath) {                   \
    auto (id) = std::make_unique<Texture>();                \
    (id)->Name = #id;                                       \
    (id)->Filename = filePath;                        \
    ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(      \
        md3dDevice.Get(),                                   \
        mCommandList.Get(), (id)->Filename.c_str(),         \
        (id)->Resource, (id)->UploadHeap));                 \
    Resources::RegularTextures[(id)->Name] = std::move(id); \
    }
    
}
