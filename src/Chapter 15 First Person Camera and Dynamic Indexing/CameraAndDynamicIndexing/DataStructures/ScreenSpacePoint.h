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
    ScreenSpacePoint(const ScreenSpacePoint& other) = default;
    ScreenSpacePoint(ScreenSpacePoint&& other) = default;
    
    ScreenSpacePoint& operator=(const ScreenSpacePoint& other) = default;

    ScreenSpacePoint operator+(const ScreenSpacePoint& other) const
    {
        return ScreenSpacePoint {this->x + other.x, this->y + other.y};
    }
};
