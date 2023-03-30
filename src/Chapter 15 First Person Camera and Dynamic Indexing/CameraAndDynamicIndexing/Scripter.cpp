#include <fstream>
#include <functional>
#include <unordered_set>
#include <memory>
#include "Scripter.h"

#include "Resources.h"
#include "Core/UIWidgets/Image2D.h"
#include "DataStructures/ChineseChar.h"


namespace
{
    /**
     * where staging Image2Ds should be placed,
     * and hide from player's view
     */
    // ScreenSpacePoint STAGING_PLACE {2000, 2000};
}

struct Scripter::ParseResult
{
private:
    typedef std::unordered_map<int, std::vector<ChineseChar>> t_DlgIdToCnChars;
    typedef std::unordered_map<int, std::vector<int>> t_DlgIdToReplies;
    typedef std::unordered_map<int, Json::Value> t_DlgToFormatConf;

public:
    /**
     * dialog's id to its present characters map
     */
    t_DlgIdToCnChars ChineseCharacters;

    t_DlgIdToReplies RepliesLookup;
    t_DlgToFormatConf FormatLookup;
};

void Scripter::Parse(const char* filePath)
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

void Scripter::Initialize(ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList, AnythingBatch* imgBatch)
{
    if (!mParseResult)
        throw std::exception("Parse() should be called before Initialize()");

    LoadAllDialogs(device, cmdList, imgBatch);
}

void Scripter::RegisterDialogHandle(int dlgIdx, t_DialogHandleFunc handleFunc)
{
    mDlgIdToHandleFunc.insert_or_assign(dlgIdx, handleFunc);
}

void Scripter::LoadDialog(int dlgIdx, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
    const std::vector<ChineseChar>& characters, AnythingBatch* imgBatch)
{
    auto text = std::make_unique<TextCN>();
    
    for (int i = 0; i < characters.size(); i++)
    {
        auto aChar = characters[i];
        
        int idx = Resources::CnCharLoader.LoadCharacter(aChar, device, cmdList);
        Texture* tex = Resources::CnCharLoader.GetByIndex(idx);

        auto x = mParseResult->FormatLookup[dlgIdx].get("x", 0);
        auto y = mParseResult->FormatLookup[dlgIdx].get("y", 0);
        auto size = mParseResult->FormatLookup[dlgIdx].get("size", 50);
        auto img = std::make_shared<Image2D>(
            ScreenSpacePoint {x.asInt() + i * size.asInt(), y.asInt()},
            size.asInt(), size.asInt(), tex
            );
        
        text->Images.push_back(img);
        imgBatch->Add(img);
    }
    
    mDlgIdToText.insert_or_assign(dlgIdx, std::move(text));
}

void Scripter::ShowDialog(int dlgIdx, const t_DialogHandleFuncParams& params)
{
    bool anyHandleFound =
        mDlgIdToHandleFunc.find(dlgIdx) != mDlgIdToHandleFunc.end();
    if (!anyHandleFound) return;

    auto& handleFunc = mDlgIdToHandleFunc[dlgIdx];
    handleFunc(params);
}

TextCN* Scripter::GetTextOfDialog(int dlgIdx)
{
    bool found = (mDlgIdToText.find(dlgIdx) != mDlgIdToText.end());
    if (!found)
        throw std::exception("Parse() & Initialize() should be called first");
    
    return mDlgIdToText[dlgIdx].get();
}

std::vector<int>& Scripter::GetReplyIndices(int dlgIdx) const
{
    return mParseResult->RepliesLookup[dlgIdx];
}

void Scripter::ParseJsonObject(const Json::Value& jsonObj)
{
    const Json::Value& dialogs = jsonObj["dialogs"];
    for (const auto& dialog : dialogs)
    {
        ParseDialog(dialog);
    }
}

void Scripter::ParseDialog(const Json::Value& dlgJson)
{
    int id          = dlgJson["id"].asInt();
    const char* say = dlgJson["say"].asCString();
    
    const Json::Value& replies = dlgJson["replies"];
    std::vector<int> replyIdxs {};
    for (auto& reply : replies)
    {
        replyIdxs.push_back(reply["id"].asInt());
        
        ParseDialog(reply);
    }
    mParseResult->RepliesLookup.insert_or_assign(id, replyIdxs);

    std::vector<ChineseChar> chars {};
    for (size_t i = 0; i < std::strlen(say); i += 2)
    {
        auto aChar = ChineseChar {say[i], say[i + 1]};
        chars.push_back(aChar);
    }
    mParseResult->ChineseCharacters.insert_or_assign(id, chars);

    mParseResult->FormatLookup.insert_or_assign(
        id, dlgJson["format"] ? dlgJson["format"] : Json::Value {}
        );
}

void Scripter::LoadAllDialogs(ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList, AnythingBatch* imgBatch)
{
    for (const auto& pair : mParseResult->ChineseCharacters)
    {
        int dlgId = pair.first;
        const std::vector<ChineseChar>& chars = pair.second;

        LoadDialog(dlgId, device, cmdList, chars, imgBatch);
    }
}
