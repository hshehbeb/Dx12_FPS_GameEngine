#pragma once
#include "Scripter.h"
#include "Core/Utilis/Image2DPlacer.h"


namespace DialogHandleFuncLibrary
{
    Image2DPlacer gPlacer {10, 50, 5};

    inline void HideText(TextCN& text)
    {
        for (const auto& img : text.Images)
            img->Visible = false;

        for (const auto& btn : Resources::gChoiceButtons)
        {
            btn->SetShouldDraw(false);
        } 
    }

    inline void HideDialog(int dlgId)
    {
        auto text = Resources::gScripter.GetTextOfDialog(dlgId);
        for (const auto & img : text->Images)
            img->Visible = false;

        auto& replies = Resources::gScripter.GetReplyIndices(0);
        for (int i = 0; i < replies.size(); i++)
        {
            auto replyIdx = replies[i];
            auto theText = Resources::gScripter.GetTextOfDialog(replyIdx);
            for (const auto & img : theText->Images)
                img->Visible = false;
            
            auto& btn = Resources::gChoiceButtons[i];
            btn->SetShouldDraw(false);
        }
    }

    inline void ShowAsClassicDialog2D(int dlgId)
    {
        auto text = Resources::gScripter.GetTextOfDialog(dlgId);
        for (const auto & img : text->Images)
            img->Visible = true;
        
        auto& replies = Resources::gScripter.GetReplyIndices(dlgId);
        for (int i = 0; i < replies.size(); i++)
        {
            auto replyIdx = replies[i];
            auto theText = Resources::gScripter.GetTextOfDialog(replyIdx);
            for (const auto & img : theText->Images)
                img->Visible = true;
            
            auto& btn = Resources::gChoiceButtons[i];
            auto pos =
                dynamic_cast<Image2D*>(theText->Images.back().get())->GetPosition()
                + ScreenSpacePoint {55, 0};
            btn->SetPosition(pos);
            btn->SetShouldDraw(true);
            btn->SetOnClickHandle([dlgId](ScreenSpacePoint) {
                HideDialog(dlgId);
                Resources::gScripter.ShowDialog(3, {});
            });
        }
    }

    inline void HandleDialog1(Scripter::t_DialogHandleFuncParams)
    {
        ShowAsClassicDialog2D(0);
    }

    inline void HandleDialog3(Scripter::t_DialogHandleFuncParams)
    {
        ShowAsClassicDialog2D(3);
    }
};
