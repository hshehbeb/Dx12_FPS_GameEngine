#pragma once

#include <d3d12.h>
#include "jsoncpp/json.h"


class ScriptParser
{
private:
    struct ParseResult;
    
public:
    void Parse(const char* filePath);
    void LoadTextureOfAllCharacters(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    
private:
    std::shared_ptr<ParseResult> mParseResult;

private:
    void ParseJsonObject(const Json::Value& jsonObj);
    void ParseDialog(const Json::Value& dlgJson);
};
