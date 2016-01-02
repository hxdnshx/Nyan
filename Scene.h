#pragma once

#include <map>

#include "MinimalAllocator.hpp"

#define MINIMAL_USE_PROCESSHEAPSTRING
#include "MinimalPath.hpp"

#define MINIMAL_USE_PROCESSHEAPARRAY
#include "MinimalArray.hpp"

#include "../../../Src/nnnEngine/nnn.h"

#include "Map.h"
#include "Line.h"

#include "VertexType.h"

static_assert(std::is_standard_layout< VertexType >::value == 1, "VertexType must be POD type");

namespace Nyan{
	using std::map;
	class Scene
	{
	protected:
		Minimal::IMinimalAllocator *m_alloc;
		Map3D *map;
		NNN::Buffer::s_VertexBuffer *m_vb;
		NNN::Buffer::s_IndexBuffer *m_ib;
		Minimal::MinimalArrayT< NNN::Texture::s_TexturePiece* > m_tptr;
		Minimal::MinimalStringT <wchar_t> m_tstr;
		size_t m_vsize, m_isize;
		void RepackTexture();
		bool m_flagrepack;
		bool m_groundflag;
		typedef std::pair < int, void* > DirtyData;
		//Minimal::MinimalArrayT< DirtyData > m_DirtyList;
		NNN::State::c_SamplerState* g_sampler_state;
		bool TestVoxel(DirectX::XMINT3);
		int AOMask(DirectX::XMINT3 dir, DirectX::XMINT3 n);
		
	public:
		typedef Minimal::MinimalArrayT < BYTE >  SaveFormat;

		int m_vremain;
		//用于存储每个体素(伪)渲染的内容在缓存中的位置
		//但是如果是为了实际渲染应该储存index而不是vertex啊- -
		std::map<int,int> m_offset;
		Minimal::MinimalArrayT< Minimal::ProcessHeapStringW > m_Texture;
		NNN::Texture::c_PackTexture m_pak;
		
		//这里有一个约定,即每个场景使用的纹理名不会相同
		//为了保证都能安全的释放
		void InitBuffer();
		void UpdateBuffer();
	
		inline void SetBlock(const int &x, const int &y, const int &z, const int val)
		{}
		void Render(int startcol,int endLayer=-1,NNN::Shader::c_Effect* effect=nullptr);
		NNN::Texture::c_Texture* GetPackedTexture(){ return m_pak.GetPackTexture(); }
		inline NNN::Texture::s_TexturePiece* const GetPiece(int id){ return m_tptr[id]; }

		void SetGroundVisiablity(bool val){ m_groundflag = val; }

		/*
			For Arg2:
				0-┌
				1-┐
				2-└
				3-┘
		*/
		inline DirectX::XMFLOAT2 GetTexloc(int texID, int loc);

		inline DirectX::XMFLOAT2 GetLoc(int loc)
		{
			return DirectX::XMFLOAT2((loc & 1) ? 1 : 0, (loc & 2) ? 1 : 0);
		}
		
		/* return Int Value*/
		DirectX::XMFLOAT4 TestCollisoin(const LineFunc& src);
		/* return Float Value*/
		DirectX::XMFLOAT4 GetIntersect(const LineFunc& src);

		//void SaveScene(__out SaveFormat &bin);
		void LoadScene(__in wchar_t* src);
		void InitScene(__in int x, __in int y, __in int z);
		void InitIndex(int lowx, int highx, int lowy, int highy, int lowz, int highz);
		inline Map3D* GetMap(){return map;}
		void ImportTexture(__in wchar_t* ptr);
		Scene(Minimal::IMinimalAllocator *alloc) :
			m_tptr(alloc), m_tstr(alloc), m_vsize(0), m_isize(0),
			m_pak(), m_Texture(alloc), m_groundflag(true)
		{
			g_sampler_state = new NNN::State::c_SamplerState();
			g_sampler_state->SetFilter(D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT);
			g_sampler_state->SetAddressU(D3D11_TEXTURE_ADDRESS_CLAMP);
			g_sampler_state->SetAddressV(D3D11_TEXTURE_ADDRESS_CLAMP);
			m_pak.Init(false);
			m_alloc = alloc;
			m_tstr = L"";
			m_vb = nullptr;
			m_ib = nullptr;
			map = nullptr;
		};
		~Scene();
	};
}