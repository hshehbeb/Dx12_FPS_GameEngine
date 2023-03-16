#pragma once

struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;
	std::string name;
	BoundingBox Bounds;
	
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	int NumFramesDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;
	GeoObject* Obj = nullptr;
	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;

public:
	void floattoWorld(float World[4][4])
	{
		  this->World._11=World[0][0];  this->World._12=World[0][1];
		  this->World._13=World[0][2];  this->World._14=World[0][3];
		  this->World._21=World[1][0];  this->World._22=World[1][1];
		  this->World._23=World[1][2];  this->World._24=World[1][3];
		  this->World._31=World[2][0];  this->World._32=World[2][1];
		  this->World._33=World[2][2];  this->World._34=World[2][3];
		  this->World._41=World[3][0];  this->World._42=World[3][1];
		  this->World._43=World[3][2];  this->World._44=World[3][3];
	}
	void floattoTrans(float TexTransform[4][4])
	{
		  this->TexTransform._11=TexTransform[0][0];   this->TexTransform._12=TexTransform[0][1];
		  this->TexTransform._13=TexTransform[0][2];   this->TexTransform._14=TexTransform[0][3];
		  this->TexTransform._21=TexTransform[1][0];   this->TexTransform._22=TexTransform[1][1];
		  this->TexTransform._23=TexTransform[1][2];   this->TexTransform._24=TexTransform[1][3];
		  this->TexTransform._31=TexTransform[2][0];   this->TexTransform._32=TexTransform[2][1];
		  this->TexTransform._33=TexTransform[2][2];   this->TexTransform._34=TexTransform[2][3];
		  this->TexTransform._41=TexTransform[3][0];   this->TexTransform._42=TexTransform[3][1];
		  this->TexTransform._43=TexTransform[3][2];   this->TexTransform._44=TexTransform[3][3];
	}
};
