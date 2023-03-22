//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float2 Uv    : UV;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float2 Uv    : UV;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	// vout.PosH = float4(vin.PosL, 1.0f);
    
    vout.Uv = vin.Uv;
    
    return vout;
}


