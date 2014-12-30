#pragma once


#include "MinimalAllocator.hpp"

#define MINIMAL_USE_PROCESSHEAPSTRING
#include "MinimalPath.hpp"

#define MINIMAL_USE_PROCESSHEAPARRAY
#include "MinimalArray.hpp"

#include "../../../Src/nnnEngine/nnn.h"
#include "VertexType.h"

namespace Nyan{
	class SelectRect
	{
	protected:
		NNN::Buffer::s_VertexBuffer *m_vb;
		NNN::Buffer::s_IndexBuffer *m_ib;
		size_t m_vsize, m_isize;
		int x, y, z, loc;
		Minimal::IMinimalAllocator *m_alloc;
		float umin, umax, vmin, vmax;
	public:
		inline DirectX::XMFLOAT2 GetTexloc(int loc);
		void SetUV(float u_min, float u_max, float v_min, float v_max)
		{
			umin = u_min;
			umax = u_max;
			vmin = v_min;
			vmax = v_max;
		}
		inline int GetX(){ return x; }
		inline int GetY(){ return y; }
		inline int GetZ(){ return z; }
		inline int GetDir(){ return loc; }
		void SetRectLocation(const int& nx, const int& ny, const int& nz, const int& direction);
		void Render(NNN::Shader::c_Effect *effect=nullptr);
		~SelectRect();
		SelectRect(Minimal::IMinimalAllocator *alloc) :x(0), y(0), z(0), loc(-1), m_alloc(alloc)
		{
			HRESULT hr;
			NNN::es_GraphAPI graph_api = NNN::GetGraphAPI();
			m_vsize = 4;
			m_isize = 6;
			m_vb = new struct NNN::Buffer::s_VertexBuffer();
			m_ib = new struct NNN::Buffer::s_IndexBuffer();

			switch (graph_api)
			{
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
			case NNN::es_GraphAPI::DX11:
				V(m_vb->Init_DX11(NULL, sizeof(VertexType)*(m_vsize), D3D11_USAGE_DYNAMIC, 0, D3D11_CPU_ACCESS_WRITE));
				V(m_ib->Init_DX11(NULL, sizeof(WORD)*(m_isize), D3D11_USAGE_DYNAMIC, 0, D3D11_CPU_ACCESS_WRITE));
				break;
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
			case NNN::es_GraphAPI::DX9:
				V(m_vb->Init_DX9(nullptr, sizeof(VertexType)*(m_vsize)));
				V(m_ib->Init_DX9(nullptr, sizeof(WORD)*(m_isize)));
				break;
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
			case NNN::es_GraphAPI::OpenGL:
				V(m_vb->Init_OpenGL(nullptr, sizeof(VertexType)*(m_vsize), GL_DYNAMIC_DRAW));
				V(m_ib->Init_OpenGL(nullptr, sizeof(WORD)*(m_isize), GL_DYNAMIC_DRAW));
				break;
#endif	// !NNN_PLATFORM_WP8
			}
		}
	};
}