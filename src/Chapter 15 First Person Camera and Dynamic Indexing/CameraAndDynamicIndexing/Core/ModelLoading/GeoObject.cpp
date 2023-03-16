#include "GeoObject.h"

#include "GeoObject.h"



GeoObject::GeoObject(std::string name)
{
    this->Name = name;
	
    SetScale(DirectX::XMMatrixScaling(1.f, 1.f, 1.f));
    m_rotate = MathHelper::Identity4x4();
    m_transform = MathHelper::Identity4x4();
    //modelRot = XMMatrixRotationZ(MathHelper::Pi / 2) * XMMatrixRotationY(MathHelper::Pi / 2);
    //modelOffset = XMMatrixTranslation(40.0f, 101.0f, 0.0f);
    //tree->SetScale(XMMatrixScaling(1.f, 1.f, 1.f));
    //tree->SetRotate(modelRot);
    //tree->SetTransform(modelOffset);

}

GeoObject::~GeoObject()
{
}

std::vector<uint32_t>& GeoObject::GetIndices32()
{
    if (mIndices32.empty())
    {
        mIndices32.resize(Indices32.size());
        for (size_t i = 0; i < Indices32.size(); ++i)
            mIndices32[i] = static_cast<uint16_t>(Indices32[i]);
    }

    return mIndices32;
}

int GeoObject::GetVertexCount() const
{
    return mVertexCount;
}

int GeoObject::GetTriangleCount() const
{
    return mTriangleCount;
}

bool GeoObject::GetDirty() const
{
    return isDirty;
}

bool GeoObject::GetTexDirty() const
{
    return isTexDirty;
}

void GeoObject::SetScale(DirectX::CXMMATRIX scale)
{
    XMStoreFloat4x4(&m_scale, scale);
}

void GeoObject::SetRotate(DirectX::CXMMATRIX rotation)
{
    XMStoreFloat4x4(&m_rotate, rotation);
}

void GeoObject::SetTransform(DirectX::CXMMATRIX transform)
{
    XMStoreFloat4x4(&m_transform, transform);
}

XMFLOAT3 GeoObject::GetScale()
{
	
    XMFLOAT3 res(m_scale._11, m_scale._22, m_scale._33);
    return res;
}

XMFLOAT3 GeoObject::GetRotate()
{
    return XMFLOAT3();
}


XMFLOAT3 GeoObject::GetTransform()
{

    XMFLOAT3 res(m_transform._41, m_transform._42, m_transform._43);
    return res;
}

DirectX::XMMATRIX GeoObject::GetWorld()
{
    XMMATRIX scale = XMLoadFloat4x4(&m_scale);
    XMMATRIX rotate = XMLoadFloat4x4(&m_rotate);
    XMMATRIX transform = XMLoadFloat4x4(&m_transform);
    return scale * rotate * transform;
}
