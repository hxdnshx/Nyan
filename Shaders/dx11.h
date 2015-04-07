struct VS_OUTPUT
{
	float4 m_Pos	: SV_POSITION;
	float3 m_rPos : POSITION0;
	float2 m_Tex : TEXCOORD0;
	float3 m_Normal : NORMAL0;
	int m_aomask : BLENDINDICES0;
	float2 m_PosOffset : TEXCOORD1;

};