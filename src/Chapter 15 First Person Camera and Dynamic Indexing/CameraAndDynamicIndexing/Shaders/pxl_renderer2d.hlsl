//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

// cbuffer cbPerObject : register(b0)
// {
// 	float4x4 gWorldViewProj; 
// };

// StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);

Texture2D tex : register(t0);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// SamplerState s1 : register(s0);

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
    float2 Uv    : UV;
};

float4 main(VertexOut pin) : SV_TARGET
{
    // return pin.Color;
    // return float4(pin.Uv, 0, 0);
    return tex.Sample(gsamLinearWrap, pin.Uv);
}


