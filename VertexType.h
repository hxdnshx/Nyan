#pragma once
#include "../../../Src/nnnEngine/nnn.h"

struct BiuVertex
{
	DirectX::XMFLOAT3 m_Pos;
	DirectX::XMFLOAT2 m_Tex;
	DirectX::XMFLOAT3 m_Normal;
	__int32 aomask;
	DirectX::XMFLOAT2 m_offset;
};

using VertexType = BiuVertex;