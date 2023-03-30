#pragma once

#include <d3d12.h>
#include <unordered_map>
#include <functional>

#include "Core/Actors/Actor.h"
#include "Core/BatchProcess/AnythingBatch.h"
#include "Core/UIWidgets/Image2D.h"
#include "DataStructures/ChineseChar.h"
#include "DataStructures/IDialogHandleFuncParam.h"
#include "jsoncpp/json.h"


class Scripter
{
private:
    struct ParseResult;

public:
    typedef std::unordered_map<std::string, IDialogHandleFuncParam> t_DialogHandleFuncParams;
    typedef std::function<void (std::vector<std::shared_ptr<IBatchable>>&, const t_DialogHandleFuncParams&)> t_DialogHandleFunc;
    
public:
    void Parse(const char* filePath);
    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    void RegisterDialogHandle(int dlgIdx, t_DialogHandleFunc handleFunc);
    void ShowDialog(int dlgIdx, AnythingBatch* imgBatch, const t_DialogHandleFuncParams& params);
    
private:
    std::shared_ptr<ParseResult> mParseResult;
    std::unordered_map<int, std::vector<std::shared_ptr<Image2D>>> mDlgIdToTextImgs;
    std::unordered_map<int, t_DialogHandleFunc> mDlgIdToHandleFunc;

private:
    void ParseJsonObject(const Json::Value& jsonObj);
    void ParseDialog(const Json::Value& dlgJson);

    void LoadAllDialogs(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void LoadDialog(int dlgIdx,
        ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
        const std::vector<ChineseChar>& characters);

};
