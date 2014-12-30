struct VS_OUTPUT
{
	float4 m_Pos	: SV_POSITION;
	float2 m_Tex : TEXCOORD0;
	float m_Normal : BLENDWEIGHT;
	int m_aomask : BLENDINDICES0;
	float2 m_PosOffset : TEXCOORD1;

};