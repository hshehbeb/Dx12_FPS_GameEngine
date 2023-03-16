#include "Model.h"


Model::~Model()
{
}

Model::Model(std::string name, std::string filePath):GeoObject(name)
{
    ObjectType = ObjType::Model;
    m_Importer = new ModelImporter(name);
    m_Importer->LoadModel(filePath);
    auto meshes = m_Importer->m_meshes;

    int indexLocation = 0;
    int vertexLocation = 0;

    XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
    XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

    XMVECTOR vMin = XMLoadFloat3(&vMinf3);
    XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

    //对于每个submesh
    for (int i = 0; i < meshes.size(); i++) {
        GeoMesh mesh;
        mesh.IndexCount = meshes[i].indices.size();
        mesh.StartIndexLocation = indexLocation;
        mesh.BaseVertexLocation = vertexLocation;

        // 材质
        Material mat;
        mat.Name = meshes[i].material.Name;
        mat.DiffuseAlbedo = meshes[i].material.DiffuseAlbedo;
        mat.Roughness = meshes[i].material.Roughness;
        // mat.DiffuseMapName = meshes[i].material.DiffuseMapName;
        // mat.NormalMapName = meshes[i].material.NormalMapName;

        mesh.material = mat;

        Meshes.push_back(mesh);

		

        // 顶点
        for (int j = 0; j < meshes[i].vertices.size(); j++) {
            Vertex v;
            v.Pos = meshes[i].vertices[j].Pos;
            v.Normal = meshes[i].vertices[j].Normal;
            v.TexC = meshes[i].vertices[j].TexC;
            Vertices.push_back(v);
			
            XMVECTOR P = XMLoadFloat3(&v.Pos);
            vMin = XMVectorMin(vMin, P);
            vMax = XMVectorMax(vMax, P);
        }

        // 索引
        Indices32.insert(Indices32.end(), meshes[i].indices.begin(), meshes[i].indices.end());

        // 更新起点
        indexLocation += meshes[i].indices.size();
        vertexLocation += meshes[i].vertices.size();
    }
    mVertexCount = Vertices.size();
    XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
    XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));
}


void Model::Update(float dt)
{
}


