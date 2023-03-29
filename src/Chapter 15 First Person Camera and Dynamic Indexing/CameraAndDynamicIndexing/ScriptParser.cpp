#include <fstream>
#include <functional>
#include <unordered_set>
#include <memory>
#include "ScriptParser.h"

#include "Resources.h"
#include "DataStructures/ChineseChar.h"


struct ScriptParser::ParseResult
{
private:
    typedef std::unordered_set<ChineseChar, std::function<std::size_t(const ChineseChar&)>> t_CnCharsSet;

public:
    t_CnCharsSet CharactersSet {0, ChineseChar::GetHashCode};

    // ParseResult(bool anyError)
    //     : AnyError(anyError)
    //     , CharactersSet(std::set<CnChar>())
    // {
    // }
};

void ScriptParser::Parse(const char* filePath)
{
    mParseResult = std::make_shared<ParseResult>();
    
    Json::Value jsonObj;
    std::ifstream ifs;
    ifs.open(filePath);

    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    if (Json::parseFromStream(builder, ifs, &jsonObj, &errs))
    {
        ParseJsonObject(jsonObj);
    }
}

void ScriptParser::LoadTextureOfAllCharacters(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    for (const auto& aChar : mParseResult->CharactersSet)
        Resources::CnCharLoader.LoadCharacter(aChar, device, cmdList);
}

void ScriptParser::ParseJsonObject(const Json::Value& jsonObj)
{
    const Json::Value& dialogs = jsonObj["dialogs"];
    for (const auto& dialog : dialogs)
    {
        ParseDialog(dialog);
    }
}

void ScriptParser::ParseDialog(const Json::Value& dlgJson)
{
    int id          = dlgJson["id"].asInt();
    const char* say = dlgJson["say"].asCString();
    
    const Json::Value& replies = dlgJson["replies"];
    for (auto& reply : replies)
    {
        ParseDialog(reply);
    }
    
    for (size_t i = 0; i < std::strlen(say); i += 2)
    {
        auto theChar = ChineseChar {say[i], say[i + 1]};
        mParseResult->CharactersSet.insert(theChar);
    }
}
