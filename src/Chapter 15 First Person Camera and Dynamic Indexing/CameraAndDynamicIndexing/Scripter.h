#pragma once

#include <d3d12.h>
#include <unordered_map>
#include <functional>

#include "Core/Actors/Actor.h"
#include "Core/BatchProcess/AnythingBatch.h"
#include "Core/UIWidgets/Image2D.h"
#include "DataStructures/ChineseChar.h"
#include "DataStructures/IDialogHandleFuncParam.h"
#include "DataStructures/TextCN.h"
#include "jsoncpp/json.h"


class Scripter
{
private:
    struct ReplyInfo
    {
        std::unique_ptr<TextCN> Text;
        int JumpToDialog;

        bool ShouldJump() const;
    };
    
    struct DialogInfo
    {
        int Id;
        std::unique_ptr<TextCN> Text;
        std::vector<std::unique_ptr<ReplyInfo>> Replies;

        void ShowDialog();
        void HideDialog();
    };    

public:
    typedef std::unordered_map<std::string, IDialogHandleFuncParam> t_DialogHandleFuncParams;
    typedef std::function<void (const t_DialogHandleFuncParams&)> t_DialogHandleFunc;

public:
    std::unordered_map<int, std::unique_ptr<DialogInfo>> DialogLookup;
    std::unordered_map<int, t_DialogHandleFunc> HandleLookup;
    
public:
    explicit Scripter(const char* filePath);
    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* imgBatch);

    void RegisterDialogHandle(int dlgIdx, t_DialogHandleFunc handleFunc);

    /**
     * it's previously registered handle function's responsibility to
     * actually decide how to render the dialog
     */
    void ShowDialog(int dlgIdx, const t_DialogHandleFuncParams& params);

    // TextCN* GetTextOfDialog(int dlgIdx);
    // std::vector<int>& GetReplyIndices(int dlgIdx) const;
    
private:
    // std::shared_ptr<ParseResult> mParseResult;
    // std::unordered_map<int, std::unique_ptr<TextCN>> mDlgIdToText;

    const char* mJsonPath;

private:
    void ParseFromJson(const char* filePath, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* imgBatch);
    void ParseJsonObject(const Json::Value& jsonObj, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* batch);
    void ParseDialog(const Json::Value& dlgJson, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* batch);
    std::unique_ptr<TextCN> CreateTextObject(const Json::Value& dlgJson, ID3D12Device* device,
                                             ID3D12GraphicsCommandList* cmdList, AnythingBatch* batch);

    // void LoadAllDialogs(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* imgBatch);
    // void LoadDialog(int dlgIdx,
    //                 ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
    //                 const std::vector<ChineseChar>& characters, AnythingBatch* imgBatch);
};
