#pragma once
#include "../../Common/d3dUtil.h"

namespace Resources
{
    static const unsigned int AvailableCharactersCount = 10;
    typedef std::array<std::unique_ptr<Texture>, AvailableCharactersCount> t_TextureArray;
    
    static t_TextureArray CharacterTextures {};
}
