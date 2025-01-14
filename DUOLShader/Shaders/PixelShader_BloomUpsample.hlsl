
struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float2 Texcoord0 : TEXCOORD0;
};

// 블러하고 난 후
Texture2D g_InputCurrTexture : register(t0);

SamplerState g_samClamp :register(s0);

float4 PSMain(VS_OUT input) : SV_TARGET
{
	float4 color = (float4)0.f;

	color = g_InputCurrTexture.Sample(g_samClamp, input.Texcoord0) /*+ g_InputCurrTexture.Sample(g_samClamp, input.Texcoord0)*/;

	return color;
}