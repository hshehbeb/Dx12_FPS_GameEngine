#pragma once

#include "../FrameResource.h"
#include "../../../Common/UploadBuffer.h"
#include "../../../Common/GeometryGenerator.h"
#include "../../../Common/MathHelper.h"
#include "../../../Common/d3dUtil.h"

namespace
{
    using namespace DirectX;
    using namespace DirectX::PackedVector;
}


class GeoMesh {
public:
    std::string name;
    UINT IndexCount;
    int StartIndexLocation;
    int BaseVertexLocation;
    Material material;

};

class GeoObject
{
public:
    GeoObject(std::string name);
    ~GeoObject();

    enum ObjType {
        Terrain = 1,
        Model
    };

    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices32;
    std::vector<GeoMesh> Meshes;

    BoundingBox bounds;

    std::string Name;
    ObjType ObjectType;

    std::vector<uint32_t>& GetIndices32();
    int GetVertexCount()const;
    int GetTriangleCount()const;
    bool GetDirty() const;
    bool GetTexDirty() const;
    virtual void Update(float dt) = 0;

    void SetScale(DirectX::CXMMATRIX scale);
    void SetRotate(DirectX::CXMMATRIX rotation);
    void SetTransform(DirectX::CXMMATRIX transform);

    XMFLOAT3 GetScale();
    XMFLOAT3 GetRotate();
    XMFLOAT3 GetTransform();
    DirectX::XMMATRIX GetWorld();

protected:
    bool isDirty = true;
    bool isTexDirty = true;
    int mVertexCount;
    int mTriangleCount;
    std::vector<uint32_t> mIndices32;
    DirectX::XMFLOAT4X4 m_scale;
    DirectX::XMFLOAT4X4 m_rotate;
    DirectX::XMFLOAT4X4 m_transform;
};

