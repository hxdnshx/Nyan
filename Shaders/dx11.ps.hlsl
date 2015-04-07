#include "dx11.h"

texture2D g_Texture;


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

	gridresult = grid(input.m_PosOffset);
	aoresult = ao(input.m_aomask, input.m_PosOffset);
	diff = input.m_Normal;//diffuse(input.m_Pos.xyz, input.m_Normal);
	ColorFin = g_Texture.Sample(MeshTextureSampler, input.m_Tex);
    return float4(ColorFin.xyz*aoresult,1.0);
}