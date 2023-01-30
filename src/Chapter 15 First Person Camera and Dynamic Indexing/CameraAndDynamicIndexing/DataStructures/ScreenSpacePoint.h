#pragma once

struct ScreenSpacePoint
{
public:
    int x, y;  /* originates from lower left corner of the screen */
    
public:
    ScreenSpacePoint(const int x, const int y)
        : x(x)
        , y(y)
    {
    }
    ScreenSpacePoint(ScreenSpacePoint& other) = default;
    ScreenSpacePoint(ScreenSpacePoint&& other) = default;
};
