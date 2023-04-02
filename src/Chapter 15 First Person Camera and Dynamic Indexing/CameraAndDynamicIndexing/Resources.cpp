#include "Resources.h"
#include "Scripter.h"

Resources::t_TextureLookup Resources::RegularTextures {};
ChineseCharactersLoader Resources::CnCharLoader {};
std::shared_ptr<Scripter> Resources::gScripter {};
std::vector<std::shared_ptr<Button>> Resources::gChoiceButtons {};


