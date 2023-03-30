#pragma once

#include <memory>
#include "../DataStructures/ScreenSpacePoint.h"
#include "../../Core/UIWidgets/Image2D.h"


class IBatchable;

class Image2DPlacer
{
private:
    typedef std::vector<std::shared_ptr<IBatchable>> t_TextImgList;

public:
    Image2DPlacer(int maxCharsInOneRow, int spacingBetweenChars, int paddingFromScreenBorder);

    void PlaceDialogAtPos(t_TextImgList& txtImages, ScreenSpacePoint atPos);
    
    void PlaceDialogAtScreenTopLeft(std::vector<std::shared_ptr<IBatchable>>& txtImages);
    void PlaceDialogAtScreenBottomLeft(std::vector<std::shared_ptr<IBatchable>>& txtImages);

private:
    int mMaxCharsInOneRow;
    int mSpacingBetweenChars;
    int mPaddingFromScreenBorder;
    
private:
    int TotalRows(int totalCharsCount) const;
};
