#include "MinimalAllocator.hpp"

#define MINIMAL_USE_PROCESSHEAPSTRING
#include "MinimalPath.hpp"

#define MINIMAL_USE_PROCESSHEAPARRAY
#include "MinimalArray.hpp"

#include "../../../Src/nnnEngine/nnn.h"

#include "Map.h"

#define VertexType NNN::Shader::ShaderLibs::Texture::ColorTexture::s_Vertex

namespace Nyan{
	class Scene
	{
	protected:
		Minimal::IMinimalAllocator *m_alloc;
		Map3D *map;
		NNN::Buffer::s_VertexBuffer *m_vb;
		NNN::Buffer::s_IndexBuffer *m_ib;
		Minimal::MinimalArrayT< NNN::Texture::c_Texture* > m_tptr;
		Minimal::MinimalStringT <wchar_t> m_tstr;
		size_t m_vsize, m_isize;
	public:
		typedef Minimal::MinimalArrayT < BYTE >  SaveFormat;
		typedef std::pair<int, int> RenderInfo;

		Minimal::MinimalArrayT< RenderInfo > m_rinfo;

		//这里有一个约定,即每个场景使用的纹理名不会相同
		//为了保证都能安全的释放
		void InitBuffer();
		void Render();
		void SaveScene(__out SaveFormat &bin);
		void LoadScene(__in SaveFormat& bin);
		void InitScene(__in int x, __in int y, __in int z);
		inline Map3D* GetMap(){return map;}
		void ImportTexture(__in wchar_t* ptr);
		Scene(Minimal::IMinimalAllocator *alloc) :
			m_tptr(alloc), m_tstr(alloc), m_rinfo(alloc)
		{
			m_tstr = L"";
			m_vb = nullptr;
			m_ib = nullptr;
			map = nullptr;
		};
		~Scene();
	};
}