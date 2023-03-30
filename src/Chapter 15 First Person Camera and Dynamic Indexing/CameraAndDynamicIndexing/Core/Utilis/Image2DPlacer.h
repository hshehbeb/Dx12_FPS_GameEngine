#pragma once

#include <memory>
#include "../DataStructures/ScreenSpacePoint.h"
#include "../../DataStructures/TextCN.h"


class IBatchable;

class Image2DPlacer
{
private:
    typedef std::vector<std::shared_ptr<ImageBase>> t_TextImgList;

public:
    Image2DPlacer(int maxCharsInOneRow, int spacingBetweenChars, int paddingFromScreenBorder);

    void PlaceTextAtPos(TextCN* text, ScreenSpacePoint atPos);

    ScreenSpacePoint ScreenTopLeft() const;
    ScreenSpacePoint ScreenBottomLeft() const;
    
    void PlaceTextAtScreenTopLeft(TextCN* text);
    void PlaceTextAtScreenBottomLeft(TextCN* text);
    // void PlaceTextAtScreenBottomRight(TextCN* text);

private:
    int mMaxCharsInOneRow;
    int mSpacingBetweenChars;
    int mPaddingFromScreenBorder;
    
private:
    int TotalRows(int totalCharsCount) const;
};
