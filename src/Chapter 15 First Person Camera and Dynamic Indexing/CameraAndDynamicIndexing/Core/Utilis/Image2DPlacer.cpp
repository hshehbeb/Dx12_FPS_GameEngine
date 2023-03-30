#include "Image2DPlacer.h"
#include "../../../Common/d3dApp.h"
#include "../../DataStructures/TextCN.h"


Image2DPlacer::Image2DPlacer(int maxCharsInOneRow, int spacingBetweenChars, int paddingFromScreenBorder)
    : mMaxCharsInOneRow(maxCharsInOneRow)
    , mSpacingBetweenChars(spacingBetweenChars)
    , mPaddingFromScreenBorder(paddingFromScreenBorder)
{
}

void Image2DPlacer::PlaceTextAtPos(TextCN* text, ScreenSpacePoint atPos)
{
    auto totalCharsCount = text->Images.size();
        
    for (int row = 0; row < TotalRows(totalCharsCount); row++)
        for (int col = 0; col < std::fmin(totalCharsCount, mMaxCharsInOneRow); col++)
            dynamic_cast<Image2D*>(text->Images[row * mMaxCharsInOneRow + col].get())->SetPosition(
                ScreenSpacePoint {
                    (atPos.x + col * mSpacingBetweenChars),
                    (atPos.y + row * mSpacingBetweenChars)
                }
            );
}

ScreenSpacePoint Image2DPlacer::ScreenTopLeft() const
{
    return ScreenSpacePoint {
        static_cast<int>(mSpacingBetweenChars * 0.5) + mPaddingFromScreenBorder / 2,
        static_cast<int>(mSpacingBetweenChars * 0.5) + mPaddingFromScreenBorder / 2
    };
}

ScreenSpacePoint Image2DPlacer::ScreenBottomLeft() const
{
    return ScreenSpacePoint {
        0, D3DApp::GetApp()->mClientHeight - mPaddingFromScreenBorder / 2
    };
}

void Image2DPlacer::PlaceTextAtScreenTopLeft(TextCN* text)
{
    PlaceTextAtPos( text, ScreenTopLeft() );
}

void Image2DPlacer::PlaceTextAtScreenBottomLeft(TextCN* text)
{
    auto charsCount = text->Images.size();
    auto pos = ScreenSpacePoint {
        static_cast<int>(mSpacingBetweenChars * 0.5) + mPaddingFromScreenBorder / 2,
        -static_cast<int>(mSpacingBetweenChars * 0.5) - (TotalRows(charsCount) - 1) * mSpacingBetweenChars + D3DApp::GetApp()->mClientHeight - mPaddingFromScreenBorder / 2
    };
    
    PlaceTextAtPos( text, pos );
}

int Image2DPlacer::TotalRows(int totalCharsCount) const
{
    return std::ceil(static_cast<float>(totalCharsCount) / mMaxCharsInOneRow);
}
