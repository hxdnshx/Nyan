#pragma once
#include "../../../Src/nnnEngine/nnn.h"

struct BiuVertex
{
	DirectX::XMFLOAT3 m_Pos;
	DirectX::XMFLOAT2 m_Tex;
};

#define VertexType NNN::Shader::ShaderLibs::Texture::ColorTexture::s_Vertex