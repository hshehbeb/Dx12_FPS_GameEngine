#include <fstream>
#include <functional>
#include <memory>
#include "Scripter.h"

#include "DialogHandleFuncLibrary.h"
#include "Resources.h"
#include "Core/UIWidgets/Image2D.h"
#include "DataStructures/ChineseChar.h"


namespace /* CONSTANTS */
{
    static const int NULL_DIALOG = -1;
}

bool Scripter::ReplyInfo::CanJumpTo() const
{
    return DialogIdJumpTo >= 0;
}

void Scripter::ParseJsonAndCreateAllDialogs(const char* filePath, ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList, AnythingBatch* imgBatch)
{
    Json::Value jsonObj;
    std::ifstream ifs;
    ifs.open(filePath);

    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    if (Json::parseFromStream(builder, ifs, &jsonObj, &errs))
    {
        ParseJsonObject(jsonObj, device, cmdList, imgBatch);
    }
}

Scripter::Scripter(const char* jsonPath)
    : mJsonPath(jsonPath)
{
}

void Scripter::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
    AnythingBatch* batchOfText, AnythingBatch* batchOfDlgBG)
{
    ParseJsonAndCreateAllDialogs(mJsonPath, device, cmdList, batchOfText);
    CreateDialogBG(batchOfDlgBG);
}

void Scripter::RegisterDialogHandle(int dlgIdx, t_DialogHandleFunc handleFunc)
{
    HandleLookup.insert_or_assign(dlgIdx, handleFunc);
}

void Scripter::ShowDialog(int dlgIdx, const t_DialogHandleFuncParams& params)
{
    bool anySpecialHandle = (HandleLookup.find(dlgIdx) != HandleLookup.end());
    anySpecialHandle ? HandleLookup[dlgIdx](params)
                     : DialogHandleFuncLibrary::ShowAsClassicDialog2D(dlgIdx);

    mDlgOnPresent = dlgIdx;
}

void Scripter::HideCurrentDialog()
{
    DialogHandleFuncLibrary::HideDialog(mDlgOnPresent);
    
    mDlgOnPresent = NULL_DIALOG;
}

void Scripter::ParseJsonObject(const Json::Value& jsonObj,
                               ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* batch)
{
    const Json::Value& dialogs = jsonObj["dialogs"];
    for (const auto& dialog : dialogs)
    {
        ParseDialog(dialog, device, cmdList, batch);
    }
}

void Scripter::ParseDialog(const Json::Value& dlgJson, ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList, AnythingBatch* batch)
{
    auto dlgInfo = std::make_unique<DialogInfo>();
    
    dlgInfo->Id   = dlgJson["id"].asInt();
    dlgInfo->Text = std::move( CreateTextObject(dlgJson, device, cmdList, batch) );
    
    const Json::Value& replies = dlgJson["replies"];
    for (auto& replyJson : replies)
    {
        auto replyInfo = std::make_unique<ReplyInfo>();
        replyInfo->Text = std::move( CreateTextObject(replyJson, device, cmdList, batch) );
        replyInfo->DialogIdJumpTo = replyJson.get("jump_to", -1).asInt();

        dlgInfo->Replies.push_back(std::move(replyInfo));
    }

    DialogLookup.insert_or_assign(dlgInfo->Id, std::move(dlgInfo));
}

std::unique_ptr<TextCN> Scripter::CreateTextObject(const Json::Value& dlgJson,
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* batch)
{
    auto text = std::make_unique<TextCN>();
    
    auto rawChars = dlgJson["say"].asCString();
    for (int i = 0; i < strlen(rawChars); i += 2)
    {
        auto aChar = ChineseChar {rawChars[i], rawChars[i + 1]};
        auto texId = Resources::CnCharLoader.LoadCharacter(aChar, device, cmdList);
        auto tex = Resources::CnCharLoader.GetByIndex(texId);

        auto x = dlgJson["format"].get("x", 0);
        auto y = dlgJson["format"].get("y", 0);
        auto size = dlgJson["format"].get("size", 50);
        auto img = std::make_shared<Image2D>(
            ScreenSpacePoint {x.asInt() + (i / 2) * size.asInt(), y.asInt()},
            size.asInt(), size.asInt(), tex
            );
        img->Visible = false;
        
        text->Images.push_back(img);
        batch->Add(img);
    }

    return std::move(text);
}

void Scripter::CreateDialogBG(AnythingBatch* batchOfDlgBG)
{
    DialogBG = std::make_shared<Image2D>(
        ScreenSpacePoint {400, 280}, 600, 360,
        Resources::RegularTextures["DialogBG"].get()
    );
    DialogBG->Visible = false;
    
    batchOfDlgBG->Add(DialogBG);
}
