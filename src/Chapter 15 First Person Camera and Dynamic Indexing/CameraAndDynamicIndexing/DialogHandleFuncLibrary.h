#pragma once
#include "Scripter.h"

namespace DialogHandleFuncLibrary
{
    void HandleDialog1(std::vector<std::shared_ptr<IBatchable>>& imgs, Scripter::t_DialogHandleFuncParams)
    {
        const int TOTAL_COLS = 10, GRID_SIZE = 50, GRID_COUNT = imgs.size();
        const int TOTAL_ROWS = std::ceil((float)GRID_COUNT/ TOTAL_COLS);
    
        for (int row = 0; row < TOTAL_ROWS; row++)
            for (int col = 0; col < std::fmin(GRID_COUNT, TOTAL_COLS); col++)
                dynamic_cast<Image2D*>(imgs[row * TOTAL_COLS + col].get())->SetPosition(
                    ScreenSpacePoint {
                        static_cast<int>(GRID_SIZE * 0.5 + col * GRID_SIZE),
                        static_cast<int>(GRID_SIZE * 0.5 + row * GRID_SIZE)
                    }
                );
    }
};
