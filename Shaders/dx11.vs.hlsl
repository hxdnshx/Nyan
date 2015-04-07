#include "dx11.h"

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix g_mWVP;
	//float4 g_Light;
}

StructuredBuffer<int> buff;

//float4 g_Light;
/*
float diffuse(float3 v, float3 n)
{
	float3 vl = normalize(g_Light.xyz - v);
		float diffuse = max(0.1, dot(vl, n));
	float dist = max(0.0, (g_Light.w - distance(g_Light.xyz, v)) / g_Light.w);
	if (any(n) == false)
	{
		return 1.0;
	}
	return diffuse*dist;
}
*/

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 a_Pos : POSITION0, float2 Tex: TEXCOORD0,float3 normal: NORMAL0, int aomask:BLENDINDICES0, float2 PosOffset : TEXCOORD1)
{
	float4 Pos = float4(a_Pos, 1.0f);
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.m_Pos	= mul(Pos, g_mWVP);
	output.m_rPos = a_Pos;
    output.m_Tex = Tex;
	output.m_Normal = normal;//diffuse(a_Pos.xyz, normal);//normal;
	output.m_aomask = buff[aomask];
	output.m_PosOffset = PosOffset;

    return output;
}