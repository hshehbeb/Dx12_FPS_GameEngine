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
public:
    struct ReplyInfo;
    struct DialogInfo;

public:
    typedef std::unordered_map<std::string, IDialogHandleFuncParam> t_DialogHandleFuncParams;
    typedef std::function<void (const t_DialogHandleFuncParams&)> t_DialogHandleFunc;

    std::unordered_map<int, std::unique_ptr<DialogInfo>> DialogLookup;
    std::unordered_map<int, t_DialogHandleFunc> HandleLookup;
    std::shared_ptr<Image2D> DialogBG;
    
public:
    explicit Scripter(const char* jsonPath);

    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* batchOfText, AnythingBatch* batchOfDlgBG);
    void RegisterDialogHandle(int dlgIdx, t_DialogHandleFunc handleFunc);

    /**
     * it's previously registered handle function's responsibility to
     * actually decide how to render the dialog
     */
    void ShowDialog(int dlgIdx, const t_DialogHandleFuncParams& params);

    void HideCurrentDialog();

private:
    const char* mJsonPath;
    int mDlgOnPresent;

private:
    void ParseJsonAndCreateAllDialogs(const char* filePath, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* imgBatch);
    void ParseJsonObject(const Json::Value& jsonObj, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* batch);
    void ParseDialog(const Json::Value& dlgJson, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, AnythingBatch* batch);
    std::unique_ptr<TextCN> CreateTextObject(const Json::Value& dlgJson, ID3D12Device* device,
                                             ID3D12GraphicsCommandList* cmdList, AnythingBatch* batch);
    void CreateDialogBG(AnythingBatch* batchOfDlgBG);
};

struct Scripter::ReplyInfo {
    std::unique_ptr<TextCN> Text;
    int DialogIdJumpTo;

    bool CanJumpTo() const;
};

struct Scripter::DialogInfo {
    int Id;
    std::unique_ptr<TextCN> Text;
    std::vector<std::unique_ptr<ReplyInfo>> Replies;

    // void ShowDialog();
    // void HideDialog();
}; 