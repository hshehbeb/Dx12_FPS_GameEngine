#pragma once
#include "Scripter.h"


namespace DialogHandleFuncLibrary
{
    inline void HideAllOptionButtons()
    {
        for (const auto& btn : Resources::gChoiceButtons)
            btn->SetShouldDraw(false);
    }

    inline void HideTextOfDialog(int dlgId)
    {
        auto* theDlg = Resources::gScripter->DialogLookup[dlgId].get();
        
        for (const auto& img : theDlg->Text->Images)
            img->Visible = false;

        for (int i = 0; i < theDlg->Replies.size(); i++)
        {
            const auto& aReply = theDlg->Replies[i];
            for (const auto & img : aReply ->Text->Images)
                img->Visible = false;

            Resources::gChoiceButtons[i]->SetShouldDraw(false);
        }
    }
    
    inline void HideDialog(int dlgId)
    {
        HideTextOfDialog(dlgId);
        HideAllOptionButtons();
        Resources::gScripter->DialogBG->Visible = false;
    }
    
    inline void ShowTextOfDialog(int dlgId)
    {
        auto* theDlg = Resources::gScripter->DialogLookup[dlgId].get();

        for (const auto& img : theDlg->Text->Images)
            img->Visible = true;

        for (int i = 0; i < theDlg->Replies.size(); i++)
        {
            const auto& aReply = theDlg->Replies[i];
            for (const auto & img : aReply ->Text->Images)
            {
                img->Visible = true;
            }

            auto& btn = Resources::gChoiceButtons[i];
            btn->SetPosition(dynamic_cast<Image2D*>(
                aReply->Text->Images.back().get())->GetPosition() + ScreenSpacePoint {50, 0}
                );
            btn->SetShouldDraw(true);
            if ( aReply->CanJumpTo() )
                btn->SetOnClickHandle([&aReply, dlgId](ScreenSpacePoint){
                    HideDialog(dlgId);
                    Resources::gScripter->ShowDialog(aReply->DialogIdJumpTo, {});
                });
            else
                btn->SetOnClickHandle([dlgId](ScreenSpacePoint){
                    HideDialog(dlgId);
                });
        }
    }
    
    inline void ShowAsClassicDialog2D(int dlgId)
    {
        ShowTextOfDialog(dlgId);
        Resources::gScripter->DialogBG->Visible = true;
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
