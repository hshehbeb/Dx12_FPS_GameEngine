#pragma once
#include <memory>
#include <vector>

#include "RenderItem.h"

class RenderItemsList
{
public:
    std::vector<std::unique_ptr<RenderItem>> Data;

public:
    RenderItemsList()
        : Data(std::vector<std::unique_ptr<RenderItem>>()), mObjCBIndex(0)
    {
    }
    ~RenderItemsList() = default;
    RenderItemsList(RenderItemsList&& other) = delete;
    RenderItemsList(RenderItemsList& other) = delete;

    void RegisterRenderItem(std::unique_ptr<RenderItem>& theRenderItem)
    {
        theRenderItem->ObjCBIndex = mObjCBIndex++;
        Data.push_back(std::move(theRenderItem));
    }

private:
    int mObjCBIndex;
};
