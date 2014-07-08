#include "Scene.h"

using namespace Nyan;

void Scene::InitScene(__in int x, __in int y, __in int z)
{
	HRESULT hr;
	NNN::es_GraphAPI graph_api = NNN::GetGraphAPI();
	map = new Map3D(m_alloc, x, y, z);
	m_vsize = x*y*z;
	m_isize = m_vsize * 6;

	switch (graph_api)
	{
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
	case NNN::es_GraphAPI::DX11:
		V(m_vb->Init_DX11(0, sizeof(VertexType), D3D11_USAGE_DYNAMIC));
		V(m_ib->Init_DX11(0, sizeof(WORD), D3D11_USAGE_DYNAMIC));
		break;
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
	case NNN::es_GraphAPI::DX9:
		V(m_vb->Init_DX9(0, sizeof(VertexType)));
		V(m_ib->Init_DX9(0, sizeof(WORD)));
		break;
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
	case NNN::es_GraphAPI::OpenGL:
		V(m_vb->Init_OpenGL(0, sizeof(VertexType), GL_DYNAMIC_DRAW));
		V(m_ib->Init_OpenGL(0, sizeof(WORD), GL_DYNAMIC_DRAW));
		break;
#endif	// !NNN_PLATFORM_WP8
	}
}

void Scene::ImportTexture(__in wchar_t* ptr)
{
	HRESULT hr;
	NNN::Texture::c_Texture* tptr;
	V(NNN::Texture::Add(ptr,ptr, 0xffff00ff, true));
	tptr = NNN::Texture::Find(ptr);
	m_tptr.Push(tptr);
}

void Scene::InitBuffer()
{
	HRESULT hr;
	NNN::es_GraphAPI graph_api = NNN::GetGraphAPI();
	if (m_vb == nullptr)
	{
		if (map == nullptr)
		{
			return;
		}
		//Precalculate buffersize
		map->SetT(m_tptr.GetSize());
		m_vsize = map->GetX()*map->GetY()*map->GetZ();
		m_vsize -= map->CalcMask();
		m_isize = m_vsize * 6;
		m_vsize *= 4;

		switch (graph_api)
		{
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
		case NNN::es_GraphAPI::DX11:
			V(m_vb->Init_DX11(0, sizeof(VertexType), D3D11_USAGE_DYNAMIC));
			V(m_ib->Init_DX11(0, sizeof(WORD),D3D11_USAGE_DYNAMIC));
			break;
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
		case NNN::es_GraphAPI::DX9:
			V(m_vb->Init_DX9(0, sizeof(VertexType)));
			V(m_ib->Init_DX9(0, sizeof(WORD)));
			break;
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
		case NNN::es_GraphAPI::OpenGL:
			V(m_vb->Init_OpenGL(0, sizeof(VertexType), GL_DYNAMIC_DRAW));
			V(m_ib->Init_OpenGL(0, sizeof(WORD), GL_DYNAMIC_DRAW));
			break;
#endif	// !NNN_PLATFORM_WP8
		}
	}
	Minimal::MinimalArrayT< VertexType > m_vlist(m_vsize,m_alloc);
	Minimal::MinimalArrayT< WORD > m_ilist(m_isize, m_alloc);
	Minimal::MinimalArrayT< int > m_cnt(m_alloc);
	m_block *ptr,*tptr;
	int x, i,cv;
	m_rinfo.Clear();
	m_rinfo.Push(std::make_pair(0, 0));
	cv = -1;
	m_cnt.Fill(m_tptr.GetSize(), 0);
	for (x = 0; x < map->GetX(); ++x)
	{
		//由于是连续的内存区域
		*ptr = map->At(x, 0, 0);
		ptr = ptr + map->GetY()*map->GetZ();
		for (i = 0; i < m_tptr.GetSize(); ++i)
		{
			tptr = map->m_FastTable[i][m_cnt[i]];
			if (tptr < ptr)
			{
				++(m_cnt[i]);
				//实际的渲染代码
			}
		}
	}
}