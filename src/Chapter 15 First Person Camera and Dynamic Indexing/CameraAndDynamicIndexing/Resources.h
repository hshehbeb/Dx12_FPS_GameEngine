#pragma once
#include "../../Common/d3dUtil.h"

namespace Resources
{
    static const unsigned int AvailableCharactersCount = 10;
    
    typedef std::array<std::unique_ptr<Texture>, AvailableCharactersCount> t_TextureArray;
    typedef std::unordered_map<std::string, std::unique_ptr<Texture>> t_TextureLookup; 
    
    static t_TextureArray CharacterTextures {};
    static t_TextureLookup RegularTextures {};
}
