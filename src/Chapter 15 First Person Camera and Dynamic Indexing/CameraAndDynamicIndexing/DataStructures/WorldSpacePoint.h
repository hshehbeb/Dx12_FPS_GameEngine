#pragma once

struct WorldSpacePoint
{
public:
    int x, y;

public:
    WorldSpacePoint(int x, int y)
        : x(x)
        , y(y)
    {
    }

    WorldSpacePoint(const WorldSpacePoint& other) = default;
    WorldSpacePoint(const WorldSpacePoint&& other) = default;
};
