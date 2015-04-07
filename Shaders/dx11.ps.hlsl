#include "dx11.h"

cbuffer ConstantBuffer : register(b0)
{
	//matrix g_mWVP;
	float4 g_LightDir;
	float4 g_Light;
}

texture2D g_Texture;
texture3D g_Map;


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

/*
vec3 vl = normalize( light_pos - v );
float diffuse  = max( 0.0, dot( vl, n ) );
final += light_color * diffuse;
*/
/*
float diffuse(float3 v, float3 n)
{
	float3 vl = normalize(g_Light.xyz - v);
	float diffuse = max(0.0, dot(vl, n));
	if (any(n)==false)
	{
		return 1.0;
	}
	return diffuse;
}
*/

SamplerState MeshTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float conelight(float4 dir)
{
	float4 ldir = normalize(float4(g_LightDir.xyz, 0.0f));
	float value = max(dot(normalize(dir), ldir), 0.1f) * g_LightDir.w;
	return value*value;
}

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

float filter(float4 side, float4 corner, float2 tc)
{
	float4 v = side.xyxy + side.zzww + corner;

	return lerp(lerp(v.x, v.y, tc.y),  lerp(v.z, v.w, tc.y), tc.x) ;
}

float grid(float2 offset)
{
	float2 tmp = float2(0.5,0.5) - abs(offset - float2(0.5, 0.5));
	if (tmp.x > 0.02 && tmp.y > 0.02)
	{
		return 1.0;
	}
	return max(min(tmp.x, tmp.y) * 50,0.5);
}

float ao(int mask, float2 offset)
{
	float4 side, corner;
	if (mask == 0)
	{
		return 1;
	}
	side = float4(
		mask & 1,
		mask & 2,
		mask & 4,
		mask & 8
		);
	corner = float4(
		mask & 16,
		mask & 32,
		mask & 64,
		mask & 128
		);
	side = min(side, float4(1.0, 1.0, 1.0, 1.0));
	corner = min(corner, float4(1.0, 1.0, 1.0, 1.0));
	return 1.0 - filter(side, corner, offset)* 0.3;
}

float4 PS( VS_OUTPUT input ) : SV_Target
{
	float4 ColorFin;
	float diff;
	float aoresult;
	float gridresult;
	float cone;
	float merge;

	cone = conelight(float4(g_Light.xyz - input.m_rPos.xyz,0.0f));
	gridresult = grid(input.m_PosOffset);
	aoresult = ao(input.m_aomask, input.m_PosOffset);
	diff = diffuse(input.m_rPos.xyz, input.m_Normal);//diffuse(input.m_Pos.xyz, input.m_Normal);
	ColorFin = g_Texture.Sample(MeshTextureSampler, input.m_Tex);
	merge = aoresult*diff;
	merge = (max(0.0, min(merge, 1.0)) * min(cone,1.0))*0.75 + 0.25;
    return float4(ColorFin.xyz*merge*gridresult,1.0);
}