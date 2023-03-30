#pragma once
#include "Scripter.h"
#include "Core/Utilis/Image2DPlacer.h"


namespace DialogHandleFuncLibrary
{
    Image2DPlacer gPlacer {10, 50, 5};
    
    void HandleDialog1(std::vector<std::shared_ptr<IBatchable>>& imgs, Scripter::t_DialogHandleFuncParams)
    {
        gPlacer.PlaceDialogAtScreenBottomLeft(imgs);
    }
};
