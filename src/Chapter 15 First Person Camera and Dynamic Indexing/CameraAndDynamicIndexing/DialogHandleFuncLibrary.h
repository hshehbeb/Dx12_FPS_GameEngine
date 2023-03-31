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
        for (int i = 0; i < replies.size(); i++)
        {
            auto replyIdx = replies[i];
            auto theText = Resources::gScripter.GetTextOfDialog(replyIdx);
            for (const auto & img : theText->Images)
                img->Visible = true;
            
            auto& btn = Resources::gChoiceButtons[i];
            auto pos =
                dynamic_cast<Image2D*>(theText->Images.back().get())->GetPosition()
                + ScreenSpacePoint {80, 0};
            btn->SetPosition(pos);
            btn->SetShouldDraw(true);
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
