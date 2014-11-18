#include "Scene.h"
#include <stdio.h>

#define scale 1.0

namespace Nyan
{
	void Scene::InitScene(__in int x, __in int y, __in int z)
	{
		HRESULT hr;
		map = new Map3D(m_alloc, x, y, z);
	}

	void Scene::LoadScene(__in wchar_t* src)
	{
		/*
			Format:
			Texture Path(Max 128 char)(relative path)
			Map File Path
			Map length(X,Y,Z)
			Texture Count(int)
		*/
		wchar_t sTexturePath[128];//MAX_PATH
		wchar_t sMapPath[128];
		wchar_t sTmpPath[128];
		int x, y, z; //Size
		int tcnt;//Texture Count
		FILE *fp;
		_wfopen_s(&fp, src, L"r");
		if (!fp)
		{
			return;
		}
		fgetws(sTexturePath, 128, fp);//没经过处理的结果存在\n,需要消除
		sTexturePath[wcslen(sTexturePath) - 1] = L'\0';
		if (ferror(fp))//If no error has occurred on stream, ferror returns 0.
		{
			assert(false);
			fclose(fp);
			return;
		}
		fgetws(sMapPath, 128, fp);
		sMapPath[wcslen(sMapPath) - 1] = L'\0';
		if (ferror(fp))//If no error has occurred on stream, ferror returns 0.
		{
			assert(false);
			fclose(fp);
			return;
		}
		fwscanf_s(fp, L"%d %d %d", &x, &y, &z);
		fwscanf_s(fp, L"%d", &tcnt);
		int i;
		for (i = 0; i < tcnt; ++i)
		{
			swprintf_s(sTmpPath, L"%s%d.png", sTexturePath, i);
			this->ImportTexture(sTmpPath);
		}
		
		InitScene(x, y, z);
		map->SetT(tcnt);
		map->LoadFromFile(sMapPath);
		fclose(fp);
		InitBuffer();
	}

	void Scene::RepackTexture()
	{
		m_flagrepack = false;
		m_tptr.Clear();
		m_pak.Repack();
		for (int i = 0; i < m_Texture.GetSize(); ++i)
		{
			m_tptr.Push(m_pak.GetTexturePiece(m_Texture[i].GetRaw()));
		}
	}

	void Scene::ImportTexture(__in wchar_t* ptr)
	{
		HRESULT hr;
		NNN::Texture::s_TexturePiece* tptr;
		V(m_pak.Add_Texture(ptr));
		m_flagrepack = true;
		m_Texture.Fill(1,ptr);
		//tptr = m_pak.GetTexturePiece(ptr);
		//m_tptr.Push(tptr);
		//map->SetT(map->GetT() + 1);
	}

	/*
	void Scene::InitBuffer(float scale)
	{
	HRESULT hr;
	NNN::es_GraphAPI graph_api = NNN::GetGraphAPI();
	int tm_vsize, tm_isize;
	//Precalculate buffersize
	map->SetT(m_tptr.GetSize());
	//tm_vsize = map->GetX()*map->GetY()*map->GetZ();
	tm_isize = tm_vsize * 6;
	tm_vsize *= 4;
	if (tm_vsize > (int)m_vsize || tm_isize > (int)m_isize)
	{
	SAFE_RELEASE(m_vb);
	SAFE_RELEASE(m_ib);
	m_vsize = tm_vsize;
	m_isize = tm_isize;
	}
	if (m_vb == nullptr)
	{
	if (map == nullptr)
	{
	return;
	}


	//NNN::SetLogError_nnnEngine(true);
	//NNN::SetLogError_nnnLib(true);
	//m_vb = nullptr;
	//m_ib = nullptr;
	m_vb = new struct NNN::Buffer::s_VertexBuffer();
	m_ib = new struct NNN::Buffer::s_IndexBuffer();

	switch (graph_api)
	{
	#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
	case NNN::es_GraphAPI::DX11:
	V(m_vb->Init_DX11(NULL, sizeof(VertexType)*m_vsize, D3D11_USAGE_DYNAMIC, 0, D3D11_CPU_ACCESS_WRITE));
	V(m_ib->Init_DX11(NULL, sizeof(WORD)*m_isize, D3D11_USAGE_DYNAMIC, 0, D3D11_CPU_ACCESS_WRITE));
	break;
	#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

	#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
	case NNN::es_GraphAPI::DX9:
	V(m_vb->Init_DX9(nullptr, sizeof(VertexType)*m_vsize));
	V(m_ib->Init_DX9(nullptr, sizeof(WORD)*m_isize));
	break;
	#endif	// NNN_PLATFORM_WIN32

	#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
	case NNN::es_GraphAPI::OpenGL:
	V(m_vb->Init_OpenGL(nullptr, sizeof(VertexType)*m_vsize, GL_DYNAMIC_DRAW));
	V(m_ib->Init_OpenGL(nullptr, sizeof(WORD)*m_isize, GL_DYNAMIC_DRAW));
	break;
	#endif	// !NNN_PLATFORM_WP8
	}
	}
	Minimal::MinimalArrayT< VertexType > m_vlist(m_alloc);
	Minimal::MinimalArrayT< WORD > m_ilist(m_alloc);
	Minimal::MinimalArrayT< int > m_cnt(m_alloc);
	VertexType vert;
	m_block *ptr, *tptr;
	int x, i, itmp;
	bool flag;
	int py, pz;
	m_vlist.Grow(m_vsize);
	m_ilist.Grow(m_isize);
	m_rinfo.Clear();
	m_rinfo.Push(std::make_pair(0, 0));
	m_cnt.Fill(m_tptr.GetSize(), 0);
	for (x = 0; x < map->GetX(); ++x)
	{
	//由于是连续的内存区域
	ptr = &(map->At(x, 0, 0));
	ptr = ptr + map->GetY()*map->GetZ();
	for (i = 0; i < (int)m_tptr.GetSize(); ++i)
	{
	flag = false;
	for (; m_cnt[i] < (int)(map->m_FastTable[i].GetSize()); ++(m_cnt[i]))
	{
	tptr = map->m_FastTable[i][m_cnt[i]];
	//当前的渲染顺序在极端情况下可能会产生涉及Alpha处渲染错误
	if (tptr < ptr)//确保X值是相同的
	{
	//++(m_cnt[i]);
	//实际的渲染代码
	//顶点格式相关代码注意
	itmp = ptr - tptr;
	pz = itmp % (map->GetZ());
	pz = pz==0 ? 0 : (map->GetZ() - pz);
	itmp += pz;
	py = itmp / map->GetZ();
	py = map->GetY() - py;
	flag = true;


	if ((tptr->mask & (Nyan::Down)) == 0)
	{
	itmp = m_vlist.GetSize();
	vert.m_Color = 0xffffffff;
	vert.m_Color_dx11_opengl = 0xffffffff;
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 0);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 1);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 2);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 3);
	m_vlist.Push(vert);
	m_ilist.Push((WORD)itmp + 3);
	m_ilist.Push((WORD)itmp + 1);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 2);
	m_ilist.Push((WORD)itmp + 3);
	}
	//continue;

	if ((tptr->mask & (Nyan::Up)) == 0)
	{
	itmp = m_vlist.GetSize();
	vert.m_Color = 0xffffffff;
	vert.m_Color_dx11_opengl = 0xffffffff;
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 0);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 1);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 2);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 3);
	m_vlist.Push(vert);
	m_ilist.Push((WORD)itmp + 3);
	m_ilist.Push((WORD)itmp + 1);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 2);
	m_ilist.Push((WORD)itmp + 3);
	}

	//continue;

	if ((tptr->mask & (Nyan::Left)) == 0)
	{
	itmp = m_vlist.GetSize();
	vert.m_Color = 0xffffffff;
	vert.m_Color_dx11_opengl = 0xffffffff;
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 0);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 1);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 2);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 3);
	m_vlist.Push(vert);
	m_ilist.Push((WORD)itmp + 3);
	m_ilist.Push((WORD)itmp + 1);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 2);
	m_ilist.Push((WORD)itmp + 3);
	}

	//continue;

	if ((tptr->mask & (Nyan::Right)) == 0)
	{
	itmp = m_vlist.GetSize();
	vert.m_Color = 0xffffffff;
	vert.m_Color_dx11_opengl = 0xffffffff;
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 0);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 1);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 2);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 3);
	m_vlist.Push(vert);
	m_ilist.Push((WORD)itmp + 3);
	m_ilist.Push((WORD)itmp + 1);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 2);
	m_ilist.Push((WORD)itmp + 3);
	}



	if ((tptr->mask & (Nyan::Front)) == 0)
	{
	itmp = m_vlist.GetSize();
	vert.m_Color = 0xffffffff;
	vert.m_Color_dx11_opengl = 0xffffffff;
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 0);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 1);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 2);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 3);
	m_vlist.Push(vert);
	m_ilist.Push((WORD)itmp + 3);
	m_ilist.Push((WORD)itmp + 1);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 2);
	m_ilist.Push((WORD)itmp + 3);
	}

	//continue;

	if ((tptr->mask & (Nyan::Back)) == 0)
	{
	itmp = m_vlist.GetSize();
	vert.m_Color = 0xffffffff;
	vert.m_Color_dx11_opengl = 0xffffffff;
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 0);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale);
	vert.m_Tex = GetTexloc(i, 1);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 2);
	m_vlist.Push(vert);
	vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale);
	vert.m_Tex = GetTexloc(i, 3);
	m_vlist.Push(vert);
	m_ilist.Push((WORD)itmp + 3);
	m_ilist.Push((WORD)itmp + 1);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 0);
	m_ilist.Push((WORD)itmp + 2);
	m_ilist.Push((WORD)itmp + 3);
	}


	}
	else
	{
	break;
	}
	}
	if (flag)
	{
	m_rinfo.Push(std::make_pair((int)m_ilist.GetSize(), (int)i));
	}
	}
	m_rinfo.Push(std::make_pair(x, -1));
	}
	m_rinfo.Push(std::make_pair(x, -1));
	assert(m_ilist.GetSize() <= m_isize);
	assert(m_vlist.GetSize() <= m_vsize);

	{
	float i;
	i = m_tptr[0]->GetMaxU();
	i = m_tptr[0]->GetMaxV();
	}

	m_vb->Map();
	m_vb->Fill_Data(m_vlist.GetRaw(), sizeof(VertexType)*m_vlist.GetSize(), 0);
	m_vb->Unmap();

	m_ib->Map();
	m_ib->Fill_Data(m_ilist.GetRaw(), sizeof(WORD)*m_ilist.GetSize(), 0);
	m_ib->Unmap();
	}
	*/

	

	void Scene::InitBuffer()
	{
		Minimal::MinimalArrayT< VertexType > m_vlist(m_alloc);
		Minimal::MinimalArrayT< WORD > m_ilist(m_alloc);
		int i, j;
		int x, py, pz;
		VertexType vert;
		int itmp;
		int length;
		m_block* tptr;

		if (m_flagrepack == true)
		{
			RepackTexture();
		}
		//Ground
		if (m_groundflag)
		{
			vert.m_Pos = DirectX::XMFLOAT3(0, 0, 0);
			vert.m_Tex = GetTexloc(7, 0);
			m_vlist.Push(vert);
			vert.m_Pos = DirectX::XMFLOAT3(0, map->GetY()*scale, 0);
			vert.m_Tex = GetTexloc(7, 1);
			m_vlist.Push(vert);
			vert.m_Pos = DirectX::XMFLOAT3(map->GetX(), 0, 0);
			vert.m_Tex = GetTexloc(7, 2);
			m_vlist.Push(vert);
			vert.m_Pos = DirectX::XMFLOAT3(map->GetX(), map->GetY(), 0);
			vert.m_Tex = GetTexloc(7, 3);
			m_vlist.Push(vert);
		}

		itmp = 0;
		m_ilist.Push((WORD)itmp + 3);
		m_ilist.Push((WORD)itmp + 1);
		m_ilist.Push((WORD)itmp + 0);
		m_ilist.Push((WORD)itmp + 0);
		m_ilist.Push((WORD)itmp + 2);
		m_ilist.Push((WORD)itmp + 3);

		m_offset.Fill(map->GetT());
		m_vlist.Grow(100 + m_vsize);
		m_ilist.Grow(100 + m_isize);
		for (i = 0; i < map->GetT(); ++i)
		{
			m_offset[i].Clear();
		}
		for (i = 0; i < map->GetT(); ++i)
		{
			for (j = 0; j < (int)map->m_FastTable[i].GetSize();++j)
			{
				tptr = &(map->m_FastTable[i][j]);
				x = tptr->x;
				py = tptr->y;
				pz = tptr->z;
				m_offset[i].Push(std::pair<int,int>(m_ilist.GetSize(),0));
				length = 0;
#if defined(Nyan_Map_EnableMaskOptimization)
#else
				tptr->mask=0;
#endif
				if ((tptr->mask & (Nyan::Down)) == 0)
				{
					++length;
					itmp = m_vlist.GetSize();
					vert.m_Color = 0xffffffff;
					vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					m_vlist.Push(vert);
					m_ilist.Push((WORD)itmp + 3);
					m_ilist.Push((WORD)itmp + 1);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 2);
					m_ilist.Push((WORD)itmp + 3);
				}
				//continue;

				if ((tptr->mask & (Nyan::Up)) == 0)
				{
					++length;
					itmp = m_vlist.GetSize();
					vert.m_Color = 0xffffffff;
					vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					m_vlist.Push(vert);
					m_ilist.Push((WORD)itmp + 3);
					m_ilist.Push((WORD)itmp + 1);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 2);
					m_ilist.Push((WORD)itmp + 3);
				}

				//continue;

				if ((tptr->mask & (Nyan::Left)) == 0)
				{
					++length;
					itmp = m_vlist.GetSize();
					vert.m_Color = 0xffffffff;
					vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					m_vlist.Push(vert);
					m_ilist.Push((WORD)itmp + 3);
					m_ilist.Push((WORD)itmp + 1);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 2);
					m_ilist.Push((WORD)itmp + 3);
				}

				//continue;

				if ((tptr->mask & (Nyan::Right)) == 0)
				{
					++length;
					itmp = m_vlist.GetSize();
					vert.m_Color = 0xffffffff;
					vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					m_vlist.Push(vert);
					m_ilist.Push((WORD)itmp + 3);
					m_ilist.Push((WORD)itmp + 1);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 2);
					m_ilist.Push((WORD)itmp + 3);
				}



				if ((tptr->mask & (Nyan::Front)) == 0)
				{
					++length;
					itmp = m_vlist.GetSize();
					vert.m_Color = 0xffffffff;
					vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					m_vlist.Push(vert);
					m_ilist.Push((WORD)itmp + 3);
					m_ilist.Push((WORD)itmp + 1);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 2);
					m_ilist.Push((WORD)itmp + 3);
				}

				//continue;

				if ((tptr->mask & (Nyan::Back)) == 0)
				{
					++length;
					itmp = m_vlist.GetSize();
					vert.m_Color = 0xffffffff;
					vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					m_vlist.Push(vert);
					m_ilist.Push((WORD)itmp + 3);
					m_ilist.Push((WORD)itmp + 1);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 2);
					m_ilist.Push((WORD)itmp + 3);
				}

				m_offset[i].Top().second = length*6;

			}
		}

		HRESULT hr;
		NNN::es_GraphAPI graph_api = NNN::GetGraphAPI();
		if (m_vlist.GetSize() > m_vsize || m_ilist.GetSize() > m_isize)
		{
			SAFE_RELEASE(m_vb);
			SAFE_RELEASE(m_ib);
			m_vsize = m_vlist.GetSize();
			m_isize = m_ilist.GetSize();
		}
		if (m_vb == nullptr)
		{
			if (map == nullptr)
			{
				return;
			}


			//NNN::SetLogError_nnnEngine(true);
			//NNN::SetLogError_nnnLib(true);
			//m_vb = nullptr;
			//m_ib = nullptr;
			m_vb = new struct NNN::Buffer::s_VertexBuffer();
			m_ib = new struct NNN::Buffer::s_IndexBuffer();

			switch (graph_api)
			{
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
			case NNN::es_GraphAPI::DX11:
				V(m_vb->Init_DX11(NULL, sizeof(VertexType)*(m_vsize+100), D3D11_USAGE_DYNAMIC, 0, D3D11_CPU_ACCESS_WRITE));
				V(m_ib->Init_DX11(NULL, sizeof(WORD)*(m_isize+600), D3D11_USAGE_DYNAMIC, 0, D3D11_CPU_ACCESS_WRITE));
				break;
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
			case NNN::es_GraphAPI::DX9:
				V(m_vb->Init_DX9(nullptr, sizeof(VertexType)*(m_vsize+100)));
				V(m_ib->Init_DX9(nullptr, sizeof(WORD)*(m_isize+600)));
				break;
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
			case NNN::es_GraphAPI::OpenGL:
				V(m_vb->Init_OpenGL(nullptr, sizeof(VertexType)*(m_vsize+100), GL_DYNAMIC_DRAW));
				V(m_ib->Init_OpenGL(nullptr, sizeof(WORD)*(m_isize+600), GL_DYNAMIC_DRAW));
				break;
#endif	// !NNN_PLATFORM_WP8
			}
		}
		m_vb->Map();
		m_vb->Fill_Data(m_vlist.GetRaw(), sizeof(VertexType)*m_vlist.GetSize(), 0);
		m_vb->Unmap();

		m_ib->Map();
		m_ib->Fill_Data(m_ilist.GetRaw(), sizeof(WORD)*m_ilist.GetSize(), 0);
		m_ib->Unmap();

		m_vremain = 100;
	}

	DirectX::XMFLOAT2 Scene::GetTexloc(int texID, int loc)
	{
		return DirectX::XMFLOAT2((loc & 0x1) > 0 ? m_tptr[texID]->m_max_u : m_tptr[texID]->m_min_u, (loc & 0x2) > 0 ? m_tptr[texID]->m_max_v : m_tptr[texID]->m_min_v);
	}

	void Scene::Render(int startLayer, int endLayer)
	{
		int i, j;
		int curx = 0;
		int startindex = 0;
		class NNN::Shader::c_Effect *effect = NNN::Shader::ShaderLibs::Texture::ColorTexture::GetEffect();



		NNN::Device::DeviceContext::IASetIndexBuffer(m_ib);
		NNN::Device::DeviceContext::IASetVertexBuffers(m_vb, sizeof(VertexType));
		NNN::Device::DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		NNN::State::SetSamplerState(g_sampler_state, 0, 0, m_pak.GetPackTexture());
		effect->SetResource("g_Texture", m_pak.GetPackTexture(),0);

		NNN::Device::DeviceContext::SetEffect(effect, NNN_SHADER_LIBS_TEXTURE_COLORTEXTURE_DX9_TECH_NAME);

		//NNN::Device::DeviceContext::Draw(4);

		/*
		for (i = 0; i < map->m_FastTable.GetSize(); ++i)
		{
			for (j = 0; j < map->m_FastTable[i].GetSize(); ++j)
			{
				auto obj = &(map->m_FastTable[i][j]);
				if (obj->TexType != -1)
				{
					auto o2 = &(m_offset[i][j]);
					NNN::Device::DeviceContext::DrawIndexed(o2->second, o2->first);
				}
			}
		}
		*/

		NNN::Device::DeviceContext::DrawIndexed(m_isize);
		
		NNN::Device::DeviceContext::EndEffect();
	}

	Scene::~Scene()
	{
		m_pak.CleanUp();
		SAFE_RELEASE(m_vb);
		SAFE_RELEASE(m_ib);
		delete map;
		delete g_sampler_state;
	}

	inline int FitFunc(int x, int y, int z, float ox,float oy, float oz)
	{//当时对每个xyz量都加了0.5,具体原因记不清了,先删掉试试
		return 32768 - (ox - x)*(ox - x) - (oy - y)*(oy - y) - (oz - z)*(oz - z);
	}

	DirectX::XMFLOAT4 Scene::TestCollisoin(const LineFunc& src)
	{
		//Note: 使用除法次数过多,如果之后需要进行优化可以从这里下手
		int e_y, e_z;
		int x, y, z;
		float tx, ty, tz;
		float ta, tb;
		int tia, tib;
		int bx = -1, by = -1, bz = -1, bplane = -1;//best
		int cplane;//CollisionPlane
		int fit = 0;
		int tmp;
		bool collflag;
		int i;
		tx = 0;
		ty = 0;
		tz = 0;
		for (x = 0; x < map->GetX(); ++x)
		{
			ta = src.p1.y + src.n.y*(x - src.p1.x) / (src.n.x); //larger
			tb = src.p1.y + src.n.y*(x + 1 - src.p1.x) / (src.n.x);

			if (ta < tb)//swap
			{
				float tmp;
				tmp = ta;
				ta = tb;
				tb = tmp;
			}

			if (ta <= -1) continue;
			if (tb >= map->GetY())continue;

			tia = ceil(ta);
			tib = floor(tb);
			if (tia > map->GetY())tia = map->GetY();
			if (tib < 0)tib = 0;
			e_y = tia;

			if (tia == tib)continue;

			for (y = tib; y < e_y; ++y)
			{
				ta = src.p1.z + src.n.z * (y - src.p1.y) / (src.n.y);//larger
				tb = src.p1.z + src.n.z * (y + 1 - src.p1.y) / (src.n.y);

				if (ta < tb)//swap
				{
					float tmp;
					tmp = ta;
					ta = tb;
					tb = tmp;
				}

				if (ta <= -1) continue;
				if (tb >= (map->GetZ()))continue;



				tia = ceil(ta);
				tib = floor(tb);
				if (tia > map->GetZ())tia = map->GetZ();
				if (tib < 0)tib = 0;
				e_z = tia;

				if (tia == tib)continue;

				for (z = tib; z < e_z; ++z)
				{
					if (map->At(x, y, z).TexType != -1)
					{
						collflag = false;
						//map->At(x, y, z).TexType = 1;
						for (i = 1; i < Nyan::Direction::Direction_MAX; i <<= 1)
						{
							if ((map->At(x, y, z).mask & i) == 0)
							{
								switch (i)
								{
								case Direction::Up:
									tz = z + 1;
									ty = (tz - src.p1.z)*src.n.y / src.n.z + src.p1.y;
									tx = (tz - src.p1.z)*src.n.x / src.n.z + src.p1.x;
									if (ty >= y && ty <= (y + 1) && tx >= x && tx <= (x + 1))
									{
										collflag = true;
									}
									break;
								case Direction::Down:
									tz = z;
									ty = (tz - src.p1.z)*src.n.y / src.n.z + src.p1.y;
									tx = (tz - src.p1.z)*src.n.x / src.n.z + src.p1.x;
									if (ty >= y && ty <= (y + 1) && tx >= x && tx <= (x + 1))
									{
										collflag = true;
									}
									break;
								case Direction::Left:
									ty = y;
									tz = (ty - src.p1.y)*src.n.z / src.n.y + src.p1.z;
									tx = (ty - src.p1.y)*src.n.x / src.n.y + src.p1.x;
									if (tz >= z && tz <= (z + 1) && tx >= x && tx <= (x + 1))
									{
										collflag = true;
									}
									break;
								case Direction::Right:
									ty = y + 1;
									tz = (ty - src.p1.y)*src.n.z / src.n.y + src.p1.z;
									tx = (ty - src.p1.y)*src.n.x / src.n.y + src.p1.x;
									if (tz >= z && tz <= (z + 1) && tx >= x && tx <= (x + 1))
									{
										collflag = true;
									}
									break;
								case Direction::Back:
									tx = x;
									tz = (tx - src.p1.x)*src.n.z / src.n.x + src.p1.z;
									ty = (tx - src.p1.x)*src.n.y / src.n.x + src.p1.y;
									if (tz >= z&&tz <= (z + 1) && ty >= y && ty <= (y + 1))
									{
										collflag = true;
									}
									break;
								case Direction::Front:
									tx = x + 1;
									tz = (tx - src.p1.x)*src.n.z / src.n.x + src.p1.z;
									ty = (tx - src.p1.x)*src.n.y / src.n.x + src.p1.y;
									if (tz >= z&&tz <= (z + 1) && ty >= y && ty <= (y + 1))
									{
										collflag = true;
									}
									break;
								}
								if (collflag)
								{
									if ((tmp = FitFunc(tx, ty, tz, src.p1.x, src.p1.y, src.p1.z)) > fit)
									{
										fit = tmp;
										bx = x;
										by = y;
										bz = z;
										bplane = i;
										//map->At(x, y, z).TexType = 2;
									}
									collflag = false;
								}
							}
						}
					}
				}
			}
		}
		//GroundCollision
		if (bplane == -1 && m_groundflag)
		{
			tz = 0;
			ty = (tz - src.p1.z)*src.n.y / src.n.z + src.p1.y;
			tx = (tz - src.p1.z)*src.n.x / src.n.z + src.p1.x;
			if (ty >= 0 && ty <= (map->GetY()) && tx >= 0 && tx <= (map->GetX()))
			{
				collflag = true;
				bx = (int)tx;
				by = (int)ty;
				bz = -1;
				bplane = Direction::Up;
			}
		}
		return DirectX::XMFLOAT4(bx, by, bz, bplane);
	}
}