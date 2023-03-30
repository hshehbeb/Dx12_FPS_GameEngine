#pragma once
#include "Scripter.h"
#include "Core/Utilis/Image2DPlacer.h"


namespace DialogHandleFuncLibrary
{
    Image2DPlacer gPlacer {10, 50, 5};

    inline void ShowAsClassicDialog2D(int dlgId)
    {
        auto text = Resources::gScripter.GetTextOfDialog(dlgId);
        for (const auto & img : text->Images)
            img->Visible = true;

        
        auto& replies = Resources::gScripter.GetReplyIndices(0);
        for (auto replyIdx : replies)
        {
            auto theText = Resources::gScripter.GetTextOfDialog(replyIdx);
            for (const auto & img : theText->Images)
                img->Visible = true;
        }
        
        // gPlacer.PlaceTextAtPos(text, ScreenSpacePoint {200, 200});

        // for (int i = 0; i < replies.size(); i++)
        // {
            // auto aReply = replies[i];
            // auto itsText = Resources::gScripter.GetTextOfDialog(aReply);
            // auto itsPos = ScreenSpacePoint {100, 300}
            //     + ScreenSpacePoint {spacingBetweenTexts * i, 0};
            //
            // gPlacer.PlaceTextAtPos( itsText, itsPos );
        // }
    }

    inline void HandleDialog1(Scripter::t_DialogHandleFuncParams)
    {
        ShowAsClassicDialog2D(0);
    }
};
