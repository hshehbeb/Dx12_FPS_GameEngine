#pragma once
#include "Scripter.h"
// #include "Core/Utilis/Image2DPlacer.h"


namespace DialogHandleFuncLibrary
{
    // Image2DPlacer gPlacer {10, 50, 5};

    inline void HideAllOptionButtons()
    {
        for (const auto& btn : Resources::gChoiceButtons)
            btn->SetShouldDraw(false);
    }
    
    inline void ShowAsClassicDialog2D(int dlgId)
    {
        Resources::gScripter->DialogLookup[dlgId]->ShowDialog();
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
