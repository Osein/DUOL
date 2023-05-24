#include "ConstantBuffer.hlsli"

#define MAX_BONE_TRANSFORM_COUNT 128
//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
//// PerObject

#ifdef USE_SKINNING
cbuffer cbPerObject : register(b2)
{
    float2 g_ObjectID;
    float g_Offset; // �ż��� �߰�. PaperBurn �� ���� Offset��.
    float g_renderFlag;

    float4 g_EffectInfo;

    Transfrom g_Transform; // : packoffset(c0);
    matrix g_BoneTransforms[MAX_BONE_TRANSFORM_COUNT]; // : packoffset(c8);
    Material g_Material; // : packoffset(c520); //8 + 4 * 128
};
#else
cbuffer cbPerObject : register(b2)
{
    float2 g_ObjectID;
    float g_Offset; // �ż��� �߰�. PaperBurn �� ���� Offset��.
    float g_renderFlag;

    float4 g_EffectInfo;

    Transfrom g_Transform; //: packoffset(c0);
    Material g_Material; //: packoffset(c8);
}
#endif
//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
Texture2D g_DiffuseTexture : register(t0);
Texture2D g_NormalMapTexture : register(t1);
Texture2D g_MetalicRoughnessTexture : register(t2);
#ifdef USE_PAPERBURN
Texture2D g_NoiseTexture : register(t3);
#endif

SamplerState g_samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------

struct PS_INPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 Tangent : TANGENT;
    float3 Normal : NORMAL;
    float2 Texcoord0 : TEXCOORD0;
    float4 matColor : Color0;
    float4 matPBR : Color1;
    uint2 objectID : TEXCOORD1;
    uint2 objectFlag : TEXCOORD2;
    uint4 Effect : Color2;
};

struct PS_OUT
{
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
    float4 World : SV_Target2;
    float4 MetalicRoughnessAOSpecular : SV_Target3;
    float4 ObjectID : SV_Target4;
    float4 Effect : SV_Target5;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUT PSMain(PS_INPUT Input) : SV_TARGET
{
    PS_OUT psOut;

#ifdef USING_ALBEDO
	psOut.Albedo = g_DiffuseTexture.Sample(g_samLinear, Input.Texcoord0);
    // gamma correction
    psOut.Albedo.xyz = pow(psOut.Albedo.xyz, 2.0f);
#else
    psOut.Albedo = float4(Input.matColor.xyz, 1.f);
#endif

#ifdef USE_PAPERBURN
    float noise = g_NoiseTexture.Sample(g_samLinear, Input.Texcoord0).x;
    float result = (noise * 10.0f/6.0f) + g_Offset; 

    float4 paperBurnColor1 = UnpackingColor(asuint(Input.Effect.z));
    float4 paperBurnColor2 = UnpackingColor(asuint(Input.Effect.w));

    if(1.0f- result < 0.0f) 
    {
	    psOut.Albedo = float4(0,0,0,1.0f);
    }
    else if(0.98f- result < 0.0f)
    {
	    psOut.Albedo = psOut.Albedo * float4(paperBurnColor1.xyz/2.0f,1.0f);
    }
    else if(0.95f- result < 0.0f)
    {
	    psOut.Albedo = psOut.Albedo * float4(paperBurnColor1.xyz,1.0f);
    }

	if(1.5f- result < 0.0f) 
    {
	    clip(-1.0f);
    }
    else if(1.49f- result < 0.0f)
    {
	    psOut.Albedo = float4(paperBurnColor2.xyz,1.0f);
    }
    else if(1.48f- result < 0.0f)
    {
	    psOut.Albedo = float4(paperBurnColor2.xyz/2.0f,1.0f);
    }
#endif
    
#ifdef USING_NORMALMAP
   float3 normalMapSample = g_NormalMapTexture.Sample(g_samLinear, Input.Texcoord0).rgb;
   psOut.Normal.xyz = pow(psOut.Normal.xyz, 2.0f);

   float3 normalT = normalMapSample * 2.0f - 1.0f;

   float3 N = Input.Normal;
   float3 T = normalize(Input.Tangent - dot(Input.Tangent, N) * N);
   float3 B = cross(N, T);

   float3x3 tanspaceMat = float3x3(T, B, N);

   psOut.Normal = float4(normalize(mul(normalT, tanspaceMat)), Input.PosH.z);
#else
    psOut.Normal = (float4(normalize(Input.Normal), Input.PosH.z));
#endif

#ifdef USING_METALICROUGHNESS
   psOut.MetalicRoughnessAOSpecular = g_MetalicRoughnessTexture.Sample(g_samLinear, Input.Texcoord0);
   psOut.MetalicRoughnessAOSpecular.w = Input.matPBR.z;
   //psOut.MetalicRoughnessAOSpecular.w = 1.f;

    // gamma correction
    psOut.MetalicRoughnessAOSpecular.xyz = pow(psOut.MetalicRoughnessAOSpecular.xyz, 2.0f);
#else
    psOut.MetalicRoughnessAOSpecular = float4(Input.matPBR.x, Input.matPBR.y, 0.0f, Input.matPBR.z);
#endif

    psOut.World = float4(Input.PosW, 1.0f);
    // Wvalue is Flag
    psOut.ObjectID = float4(asfloat(Input.objectID.xy), asfloat(Input.objectFlag.xy));
    psOut.Effect = asfloat(Input.Effect);

    return psOut;
}