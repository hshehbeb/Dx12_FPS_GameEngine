#include <fstream>
#include <functional>
#include <memory>
#include "Scripter.h"

#include "DialogHandleFuncLibrary.h"
#include "Resources.h"
#include "Core/UIWidgets/Image2D.h"
#include "DataStructures/ChineseChar.h"


bool Scripter::ReplyInfo::ShouldJump() const
{
    return JumpToDialog >= 0;
}

void Scripter::DialogInfo::ShowDialog()
{
    for (const auto & img : Text->Images)
        img->Visible = true;

    for (int i = 0; i < Replies.size(); i++)
    {
        const auto& aReply = Replies[i];
        for (const auto & img : aReply ->Text->Images)
        {
            img->Visible = true;
        }

        auto& btn = Resources::gChoiceButtons[i];
        btn->SetPosition(dynamic_cast<Image2D*>(
            aReply->Text->Images.back().get())->GetPosition() + ScreenSpacePoint {50, 0}
            );
        btn->SetShouldDraw(true);
        if ( aReply->ShouldJump() )
            btn->SetOnClickHandle([&aReply, this](ScreenSpacePoint){
                this->HideDialog();
                Resources::gScripter->ShowDialog(aReply->JumpToDialog, {});
            });
    }
}

void Scripter::DialogInfo::HideDialog()
{
    for (const auto& img : Text->Images)
        img->Visible = false;

    for (int i = 0; i < Replies.size(); i++)
    {
        const auto& aReply = Replies[i];
        for (const auto & img : aReply ->Text->Images)
            img->Visible = false;

        Resources::gChoiceButtons[i]->SetShouldDraw(false);
    }
}

// struct Scripter::ParseResult
// {
// private:
//     typedef std::unordered_map<int, std::vector<ChineseChar>> t_DlgIdToCnChars;
//     typedef std::unordered_map<int, std::vector<int>> t_DlgIdToReplies;
//     typedef std::unordered_map<int, Json::Value> t_DlgToFormatConf;
//
// public:
//     /**
//      * dialog's id to its present characters map
//      */
//     // t_DlgIdToCnChars ChineseCharacters;
//
//     std::vector<std::unique_ptr<DialogInfo>> AllDialogs;
//     std::unordered_map<int, DialogInfo*> DialogLookup; 
//
//     // t_DlgIdToReplies RepliesLookup;
//     t_DlgToFormatConf FormatLookup;
// };

void Scripter::ParseFromJson(const char* filePath, ID3D12Device* device,
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

Scripter::Scripter(const char* filePath)
    : mJsonPath(filePath)
{
}

void Scripter::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* imgBatch)
{
    // if (!mParseResult)
        // throw std::exception("Parse() should be called before Initialize()");

    ParseFromJson(mJsonPath, device, cmdList, imgBatch);
    
    // LoadAllDialogs(device, cmdList, imgBatch);
}

void Scripter::RegisterDialogHandle(int dlgIdx, t_DialogHandleFunc handleFunc)
{
    HandleLookup.insert_or_assign(dlgIdx, handleFunc);
}

// void Scripter::LoadDialog(int dlgIdx, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
//     const std::vector<ChineseChar>& characters, AnythingBatch* imgBatch)
// {
//     auto text = std::make_unique<TextCN>();
//     
//     for (int i = 0; i < characters.size(); i++)
//     {
//         auto aChar = characters[i];
//         
//         int idx = Resources::CnCharLoader.LoadCharacter(aChar, device, cmdList);
//         Texture* tex = Resources::CnCharLoader.GetByIndex(idx);
//
//         auto x = mParseResult->FormatLookup[dlgIdx].get("x", 0);
//         auto y = mParseResult->FormatLookup[dlgIdx].get("y", 0);
//         auto size = mParseResult->FormatLookup[dlgIdx].get("size", 50);
//         auto img = std::make_shared<Image2D>(
//             ScreenSpacePoint {x.asInt() + i * size.asInt(), y.asInt()},
//             size.asInt(), size.asInt(), tex
//             );
//         img->Visible = false;
//         
//         text->Images.push_back(img);
//         imgBatch->Add(img);
//     }
//     
//     mDlgIdToText.insert_or_assign(dlgIdx, std::move(text));
// }

void Scripter::ShowDialog(int dlgIdx, const t_DialogHandleFuncParams& params)
{
    bool anyHandleFound = (HandleLookup.find(dlgIdx) != HandleLookup.end());

    anyHandleFound
        ? HandleLookup[dlgIdx](params)
        : DialogHandleFuncLibrary::ShowAsClassicDialog2D(dlgIdx); 
}

// TextCN* Scripter::GetTextOfDialog(int dlgIdx)
// {
//     bool found = (mDlgIdToText.find(dlgIdx) != mDlgIdToText.end());
//     if (!found)
//         throw std::exception("Parse() & Initialize() should be called first");
//     
//     return mDlgIdToText[dlgIdx].get();
// }

// std::vector<int>& Scripter::GetReplyIndices(int dlgIdx) const
// {
//     return mParseResult->RepliesLookup[dlgIdx];
// }

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
        replyInfo->JumpToDialog = replyJson.get("jump_to", -1).asInt();

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

// void Scripter::LoadAllDialogs(ID3D12Device* device,
//     ID3D12GraphicsCommandList* cmdList, AnythingBatch* imgBatch)
// {
//     for (const auto& pair : mParseResult->ChineseCharacters)
//     {
//         int dlgId = pair.first;
//         const std::vector<ChineseChar>& chars = pair.second;
//
//         LoadDialog(dlgId, device, cmdList, chars, imgBatch);
//     }
// }
