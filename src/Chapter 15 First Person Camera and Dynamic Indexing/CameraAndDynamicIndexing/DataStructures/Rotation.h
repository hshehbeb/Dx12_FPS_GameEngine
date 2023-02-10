#pragma once
#include <algorithm>
#include <complex>
#include <DirectXCollision.h>

class Rotation
{
public:
    DirectX::XMFLOAT3 EulerAngle;
    
public:
    explicit Rotation(float roll, float pitch, float yaw)
        : EulerAngle({ roll, pitch, yaw })
    {
    }
    
    explicit Rotation(DirectX::XMFLOAT3 eulerAngle)
        : EulerAngle(eulerAngle)
    {
    }

    DirectX::XMMATRIX CalculateMatrix()
    {
        const float PI = 3.141592653589;
        const float r = EulerAngle.x * PI / 180.0f;
        const float y = EulerAngle.z * PI / 180.0f;
        const float p = EulerAngle.y * PI / 180.0f;

        using namespace std;
        return DirectX::XMMATRIX
        {
            cos(y) * cos(r), cos(y) * sin(r), -sin(y), 0,
            sin(p) * sin(y) * cos(r) - cos(p) * sin(r), sin(p) * sin(y) * sin(r) + cos(p) * cos(r), sin(p) * cos(y), 0,
            sin(y) * cos(p) * cos(r) + sin(p) * sin(r), sin(y) * sin(r) * cos(p) - sin(p) * cos(r), cos(p) * cos(y), 0,
            0, 0, 0, 1
        };
    }
};
