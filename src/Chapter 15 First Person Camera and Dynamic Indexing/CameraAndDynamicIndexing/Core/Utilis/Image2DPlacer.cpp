#include "Image2DPlacer.h"
#include "../../../Common/d3dApp.h"


Image2DPlacer::Image2DPlacer(int maxCharsInOneRow, int spacingBetweenChars, int paddingFromScreenBorder): mMaxCharsInOneRow(maxCharsInOneRow),
    mSpacingBetweenChars(spacingBetweenChars),
    mPaddingFromScreenBorder(paddingFromScreenBorder)
{
}

void Image2DPlacer::PlaceDialogAtPos(t_TextImgList& txtImages, ScreenSpacePoint atPos)
{
    auto totalCharsCount = txtImages.size();
        
    for (int row = 0; row < TotalRows(totalCharsCount); row++)
        for (int col = 0; col < std::fmin(totalCharsCount, mMaxCharsInOneRow); col++)
            dynamic_cast<Image2D*>(txtImages[row * mMaxCharsInOneRow + col].get())->SetPosition(
                ScreenSpacePoint {
                    (atPos.x + col * mSpacingBetweenChars),
                    (atPos.y + row * mSpacingBetweenChars)
                }
            );
}

void Image2DPlacer::PlaceDialogAtScreenTopLeft(std::vector<std::shared_ptr<IBatchable>>& txtImages)
{
    PlaceDialogAtPos(
        txtImages,
        ScreenSpacePoint {
            static_cast<int>(mSpacingBetweenChars * 0.5) + mPaddingFromScreenBorder / 2,
            static_cast<int>(mSpacingBetweenChars * 0.5) + mPaddingFromScreenBorder / 2
        }
    );
}

void Image2DPlacer::PlaceDialogAtScreenBottomLeft(std::vector<std::shared_ptr<IBatchable>>& txtImages)
{
    auto totalCharsCount = txtImages.size();

    PlaceDialogAtPos(
        txtImages,
        ScreenSpacePoint {
            static_cast<int>(mSpacingBetweenChars * 0.5) + mPaddingFromScreenBorder / 2,
            -static_cast<int>(mSpacingBetweenChars * 0.5) - (TotalRows(totalCharsCount) - 1) * mSpacingBetweenChars + D3DApp::GetApp()->mClientHeight - mPaddingFromScreenBorder / 2}
    );
}

int Image2DPlacer::TotalRows(int totalCharsCount) const
{
    return std::ceil(static_cast<float>(totalCharsCount) / mMaxCharsInOneRow);
}
