cbuffer owoContantBuffer : register (b0)
{

	float4x4 g_mWVP;
}

texture2D g_Texture;

struct VS_INPUT
{
	float3 Position : POSITION0;
	float2 TexLoc : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION0;
	float2 TexLoc : TEXCOORD0;
};

VS_OUTPUT biuVS(VS_INPUT dataIn)
{
	VS_OUTPUT result;
	float4 vPos = float4(dataIn.Position, 1.0f);

	vPos = mul(vPos, g_mWVP);
	result.Position = vPos;
	result.TexLoc = dataIn.TexLoc;
	return result;
}

SamplerState MeshTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 biuPS(VS_OUTPUT dataIn) : SV_Target
{
	float4 ColorFin;
	ColorFin = g_Texture.Sample(MeshTextureSampler, dataIn.TexLoc);

	return ColorFin;
}

technique11 DefaultTech
{
	pass Pass0
	{
		SetGeometryShader(NULL);
		SetVertexShader(CompileShader(vs_5_0, biuVS()));
		SetPixelShader(CompileShader(ps_5_0, biuPS()));
	}
};