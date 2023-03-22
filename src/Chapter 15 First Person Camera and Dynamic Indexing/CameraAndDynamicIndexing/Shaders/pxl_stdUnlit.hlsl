// arthur: sherlockcai
// date:   2023.3

#define ALPHA_TEST

Texture2D tex : register(t0);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float2 Uv    : UV;
};

float4 main(VertexOut pin) : SV_TARGET
{
    float4 sampleColor = tex.Sample(gsamLinearWrap, pin.Uv);;

#ifdef ALPHA_TEST
    clip(sampleColor.a - 0.1f);
#endif
    
    return sampleColor;
}


