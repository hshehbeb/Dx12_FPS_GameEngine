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
    ScreenSpacePoint STAGING_PLACE {2000, 2000};
}

struct Scripter::ParseResult
{
private:
    typedef std::unordered_map<int, std::vector<ChineseChar>> t_IdToCnChars;

public:
    /**
     * dialog's id to its present characters map
     */
    t_IdToCnChars ChineseCharacters {0};
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

void Scripter::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    if (!mParseResult)
        throw std::exception("Parse() should be called before Initialize()");

    LoadAllDialogs(device, cmdList);
}

void Scripter::RegisterDialogHandle(int dlgIdx, t_DialogHandleFunc handleFunc)
{
    mDlgIdToHandleFunc.insert_or_assign(dlgIdx, handleFunc);
}

void Scripter::LoadDialog(int dlgIdx, ID3D12Device* device,
                          ID3D12GraphicsCommandList* cmdList, const std::vector<ChineseChar>& characters)
{
    std::vector<std::shared_ptr<Image2D>> textImages;
    for (const ChineseChar& aChar : characters)
    {
        int idx = Resources::CnCharLoader.LoadCharacter(aChar, device, cmdList);
        Texture* tex = Resources::CnCharLoader.GetByIndex(idx);

        auto img = std::make_shared<Image2D>(STAGING_PLACE, 50, 50, tex);
        img->Initialize(device, cmdList);
        
        textImages.push_back(img);
    }
    
    mDlgIdToTextImgs.insert_or_assign(dlgIdx, std::move(textImages));
}

void Scripter::ShowDialog(int dlgIdx, AnythingBatch* imgBatch,
    const t_DialogHandleFuncParams& params)
{
    bool anyImgFound =
        mDlgIdToTextImgs.find(dlgIdx) != mDlgIdToTextImgs.end();
    if (!anyImgFound)
        throw std::exception("Parse() & Initialize() should be called first");

    std::vector<std::shared_ptr<IBatchable>> imgs {};
    for (auto& img : mDlgIdToTextImgs[dlgIdx])
    {
        imgs.push_back(img);
    }
    imgBatch->AddRange(imgs);

    
    bool anyHandleFound =
        mDlgIdToHandleFunc.find(dlgIdx) != mDlgIdToHandleFunc.end();
    if (!anyHandleFound) return;

    auto& handleFunc = mDlgIdToHandleFunc[dlgIdx];
    handleFunc(imgs, params);
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
    for (auto& reply : replies)
    {
        ParseDialog(reply);
    }

    std::vector<ChineseChar> chars {};
    for (size_t i = 0; i < std::strlen(say); i += 2)
    {
        auto aChar = ChineseChar {say[i], say[i + 1]};
        chars.push_back(aChar);
    }
    mParseResult->ChineseCharacters.insert_or_assign(id, chars);
}

void Scripter::LoadAllDialogs(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    for (const auto& pair : mParseResult->ChineseCharacters)
    {
        int dlgId = pair.first;
        const std::vector<ChineseChar>& chars = pair.second;

        LoadDialog(dlgId, device, cmdList, chars);
    }
}
