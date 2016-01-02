#include "Scene.h"
#include <stdio.h>

#define scale 1.0

using namespace DirectX;

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
		fgetws(sTexturePath, 128, fp);//û��������Ľ������\n,��Ҫ����
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
	//�������������ڴ�����
	ptr = &(map->At(x, 0, 0));
	ptr = ptr + map->GetY()*map->GetZ();
	for (i = 0; i < (int)m_tptr.GetSize(); ++i)
	{
	flag = false;
	for (; m_cnt[i] < (int)(map->m_FastTable[i].GetSize()); ++(m_cnt[i]))
	{
	tptr = map->m_FastTable[i][m_cnt[i]];
	//��ǰ����Ⱦ˳���ڼ�������¿��ܻ�����漰Alpha����Ⱦ����
	if (tptr < ptr)//ȷ��Xֵ����ͬ��
	{
	//++(m_cnt[i]);
	//ʵ�ʵ���Ⱦ����
	//�����ʽ��ش���ע��
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

	
	int Scene::AOMask(DirectX::XMINT3 dir,DirectX::XMINT3 n)
	{
		int ret=0;
		DirectX::XMINT3 dir1, dir2;
		/*
			n			xyz
			dir1	yzx
			dir2	zxy
		*/
		
		dir.x += n.x;
		dir.y += n.y;
		dir.z += n.z;
		n.x = abs(n.x);
		n.y = abs(n.y);
		n.z = abs(n.z);
		dir1.x = dir2.z = n.y;
		dir1.y = dir2.x = n.z;
		dir1.z = dir2.y = n.x;
		ret += (TestVoxel(XMINT3(dir.x - dir1.x, dir.y - dir1.y, dir.z - dir1.z)) ? 1 : 0) * 1;
		ret += (TestVoxel(XMINT3(dir.x + dir1.x, dir.y + dir1.y, dir.z + dir1.z)) ? 1 : 0) * 2;
		ret += (TestVoxel(XMINT3(dir.x - dir2.x, dir.y - dir2.y, dir.z - dir2.z)) ? 1 : 0) * 4;
		ret += (TestVoxel(XMINT3(dir.x + dir2.x, dir.y + dir2.y, dir.z + dir2.z)) ? 1 : 0) * 8;

		ret += (TestVoxel(XMINT3(dir.x - dir1.x - dir2.x, dir.y - dir1.y - dir2.y, dir.z - dir1.z - dir2.z)) ? 1 : 0) * 16;
		ret += (TestVoxel(XMINT3(dir.x + dir1.x - dir2.x, dir.y + dir1.y - dir2.y, dir.z + dir1.z - dir2.z)) ? 1 : 0) * 32;
		ret += (TestVoxel(XMINT3(dir.x - dir1.x + dir2.x, dir.y - dir1.y + dir2.y, dir.z - dir1.z + dir2.z)) ? 1 : 0) * 64;
		ret += (TestVoxel(XMINT3(dir.x + dir1.x + dir2.x, dir.y + dir1.y + dir2.y, dir.z + dir1.z + dir2.z)) ? 1 : 0) * 128;
		return ret;
	}

	inline bool Scene::TestVoxel(DirectX::XMINT3 dat)
	{
		return map->GetBlock_IgnoreBound(dat.x, dat.y, dat.z).TexType != -1;
	}

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
			vert.m_Normal = DirectX::XMFLOAT3(0, 0, 1);
			vert.m_Pos = DirectX::XMFLOAT3(0, 0, 0);
			vert.m_Tex = GetTexloc(7, 0);
			vert.aomask = 0;
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

		m_vlist.Grow(100 + m_vsize);
		m_ilist.Grow(100 + m_isize);
		m_offset.clear();
		for (i = 0; i < map->GetT(); ++i)
		{
			for (j = 0; j < (int)map->m_FastTable[i].GetSize();++j)
			{
				tptr = &(map->m_FastTable[i][j]);
				x = tptr->x;
				py = tptr->y;
				pz = tptr->z;
				length = 0;
				m_offset[map->GetZ()*map->GetY()*x + map->GetZ()*py + pz] = m_vlist.GetSize();
#if defined(Nyan_Map_EnableMaskOptimization)
#else
				tptr->mask=0;
#endif
				if ((tptr->mask & (Nyan::Down)) == 0)
				{
					++length;
					itmp = m_vlist.GetSize();
					//vert.m_Color = 0xffffffff;
					//vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Normal = DirectX::XMFLOAT3(0, 0, -1);
					vert.aomask = AOMask(XMINT3(x, py, pz), XMINT3(vert.m_Normal.x, vert.m_Normal.y, vert.m_Normal.z));
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					vert.m_offset = GetLoc(1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					vert.m_offset = GetLoc(3);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					vert.m_offset = GetLoc(0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					vert.m_offset = GetLoc(2);
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
					//vert.m_Color = 0xffffffff;
					//vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Normal = DirectX::XMFLOAT3(0, 0, 1);
					vert.aomask = AOMask(XMINT3(x, py, pz), XMINT3(vert.m_Normal.x, vert.m_Normal.y, vert.m_Normal.z));
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					vert.m_offset = GetLoc(0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					vert.m_offset = GetLoc(2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					vert.m_offset = GetLoc(1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					vert.m_offset = GetLoc(3);
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
					//vert.m_Color = 0xffffffff;
					//vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Normal = DirectX::XMFLOAT3(0, -1, 0);
					vert.aomask = AOMask(XMINT3(x, py, pz), XMINT3(vert.m_Normal.x, vert.m_Normal.y, vert.m_Normal.z));
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					vert.m_offset = GetLoc(1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					vert.m_offset = GetLoc(3);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					vert.m_offset = GetLoc(0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					vert.m_offset = GetLoc(2);
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
					//vert.m_Color = 0xffffffff;
					//vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Normal = DirectX::XMFLOAT3(0, 1, 0);
					vert.aomask = AOMask(XMINT3(x, py, pz), XMINT3(vert.m_Normal.x, vert.m_Normal.y, vert.m_Normal.z));
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					vert.m_offset = GetLoc(3);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					vert.m_offset = GetLoc(1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					vert.m_offset = GetLoc(2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					vert.m_offset = GetLoc(0);
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
					//vert.m_Color = 0xffffffff;
					//vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Normal = DirectX::XMFLOAT3(1, 0, 0);
					vert.aomask = AOMask(XMINT3(x, py, pz), XMINT3(vert.m_Normal.x, vert.m_Normal.y, vert.m_Normal.z));
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					vert.m_offset = GetLoc(2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					vert.m_offset = GetLoc(3);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					vert.m_offset = GetLoc(0);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					vert.m_offset = GetLoc(1);
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
					//vert.m_Color = 0xffffffff;
					//vert.m_Color_dx11_opengl = 0xffffffff;
					vert.m_Normal = DirectX::XMFLOAT3(-1, 0, 0);
					vert.aomask = AOMask(XMINT3(x, py, pz), XMINT3(vert.m_Normal.x, vert.m_Normal.y, vert.m_Normal.z));
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 0);
					vert.m_offset = GetLoc(3);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale);
					vert.m_Tex = GetTexloc(i, 1);
					vert.m_offset = GetLoc(2);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 2);
					vert.m_offset = GetLoc(1);
					m_vlist.Push(vert);
					vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale);
					vert.m_Tex = GetTexloc(i, 3);
					vert.m_offset = GetLoc(0);
					m_vlist.Push(vert);
					m_ilist.Push((WORD)itmp + 3);
					m_ilist.Push((WORD)itmp + 1);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 0);
					m_ilist.Push((WORD)itmp + 2);
					m_ilist.Push((WORD)itmp + 3);
				}

				//m_offset[i].Top().second = length*6;

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
				assert(false);//Unaviliable running in DX9 Mode Currently
				V(m_vb->Init_DX9(nullptr, sizeof(VertexType)*(m_vsize+100)));
				V(m_ib->Init_DX9(nullptr, sizeof(WORD)*(m_isize+600)));
				break;
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
			case NNN::es_GraphAPI::OpenGL:
				assert(false);//Unaviliable running in OpenGL Mode currently
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

	void Scene::InitIndex(int lowx, int highx, int lowy, int highy, int lowz, int highz)
	{
		//Minimal::MinimalArrayT< VertexType > m_vlist(m_alloc);
		Minimal::MinimalArrayT< WORD > m_ilist(m_alloc);
		//int i, j;
		int x, py, pz;
		//VertexType vert;
		int itmp;
		const m_block* tptr;

		itmp = 0;
	
		//Ground
		if (m_groundflag)
		{
			m_ilist.Push((WORD)itmp + 3);
			m_ilist.Push((WORD)itmp + 1);
			m_ilist.Push((WORD)itmp + 0);
			m_ilist.Push((WORD)itmp + 0);
			m_ilist.Push((WORD)itmp + 2);
			m_ilist.Push((WORD)itmp + 3);
		}

		m_ilist.Grow(100 + m_isize);
		m_offset.clear();
		for (x = lowx; x < highx; ++x)
		{
			for (py = lowy; py < highy; ++py)
			{
				for (pz = lowz; pz < highz; ++pz)
				{
					tptr = &(map->At(x, py, pz));
					if (tptr->TexType == -1)continue;
					itmp = m_offset[map->GetZ()*map->GetY()*x + map->GetZ()*py + pz];
#if defined(Nyan_Map_EnableMaskOptimization)
#else
					tptr->mask = 0;
#endif
					if ((tptr->mask & (Nyan::Down)) == 0)
					{
						//vert.m_Color = 0xffffffff;
						//vert.m_Color_dx11_opengl = 0xffffffff;

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
						m_ilist.Push((WORD)itmp + 3);
						m_ilist.Push((WORD)itmp + 1);
						m_ilist.Push((WORD)itmp + 0);
						m_ilist.Push((WORD)itmp + 0);
						m_ilist.Push((WORD)itmp + 2);
						m_ilist.Push((WORD)itmp + 3);
					}



					if ((tptr->mask & (Nyan::Front)) == 0)
					{
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
						m_ilist.Push((WORD)itmp + 3);
						m_ilist.Push((WORD)itmp + 1);
						m_ilist.Push((WORD)itmp + 0);
						m_ilist.Push((WORD)itmp + 0);
						m_ilist.Push((WORD)itmp + 2);
						m_ilist.Push((WORD)itmp + 3);
					}

					//m_offset[i].Top().second = length*6;
				}
			}
		}

		HRESULT hr;
		NNN::es_GraphAPI graph_api = NNN::GetGraphAPI();
		if (m_ilist.GetSize() > m_isize)
		{
			SAFE_RELEASE(m_ib);
			m_isize = m_ilist.GetSize();
		}
		if (m_ib == nullptr)
		{
			if (map == nullptr)
			{
				return;
			}


			//NNN::SetLogError_nnnEngine(true);
			//NNN::SetLogError_nnnLib(true);
			//m_vb = nullptr;
			//m_ib = nullptr;
			m_ib = new struct NNN::Buffer::s_IndexBuffer();

			switch (graph_api)
			{
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
			case NNN::es_GraphAPI::DX11:
				V(m_ib->Init_DX11(NULL, sizeof(WORD)*(m_isize + 600), D3D11_USAGE_DYNAMIC, 0, D3D11_CPU_ACCESS_WRITE));
				break;
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
			case NNN::es_GraphAPI::DX9:
				assert(false);//Unaviliable running in DX9 Mode Currently
				V(m_ib->Init_DX9(nullptr, sizeof(WORD)*(m_isize + 600)));
				break;
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
			case NNN::es_GraphAPI::OpenGL:
				assert(false);//Unaviliable running in OpenGL Mode currently
				V(m_ib->Init_OpenGL(nullptr, sizeof(WORD)*(m_isize + 600), GL_DYNAMIC_DRAW));
				break;
#endif	// !NNN_PLATFORM_WP8
			}
		}

		m_ib->Map();
		m_ib->Fill_Data(m_ilist.GetRaw(), sizeof(WORD)*m_ilist.GetSize(), 0);
		m_ib->Unmap();
	}

	DirectX::XMFLOAT2 Scene::GetTexloc(int texID, int loc)
	{
		return DirectX::XMFLOAT2((loc & 0x1) > 0 ? m_tptr[texID]->m_max_u : m_tptr[texID]->m_min_u, (loc & 0x2) > 0 ? m_tptr[texID]->m_max_v : m_tptr[texID]->m_min_v);
	}

	void Scene::Render(int startLayer, int endLayer, NNN::Shader::c_Effect *effect)
	{
		int i, j;
		int curx = 0;
		int startindex = 0;
		//class NNN::Shader::c_Effect *effect = NNN::Shader::ShaderLibs::Texture::ColorTexture::GetEffect();



		NNN::Device::DeviceContext::IASetIndexBuffer(m_ib);
		NNN::Device::DeviceContext::IASetVertexBuffers(m_vb, sizeof(VertexType));
		NNN::Device::DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		NNN::State::SetSamplerState(g_sampler_state, 0, 0, m_pak.GetPackTexture());
		effect->SetResource("g_Texture", m_pak.GetPackTexture(),0);
	
		NNN::Device::DeviceContext::SetEffect(effect);

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
	{//��ʱ��ÿ��xyz��������0.5,����ԭ��ǲ�����,��ɾ������
		return 32768 - (ox - x)*(ox - x) - (oy - y)*(oy - y) - (oz - z)*(oz - z);
	}



	DirectX::XMFLOAT4 Scene::GetIntersect(const LineFunc& src)
	{
		/*
		// grid space
		vec3 grid = floor( pos ); //����ȡ����������������ʹ��
		vec3 grid_step = sign( dir ); //��ȡdir(����)����������<-��˼����˵��ȡ����Ĳ�������,��Ȼֻ������
		vec3 corner = max( grid_step, vec3( 0.0 ) );//����->0 //Ӧ��������ڲ��������ϲ�����ƫ��ֵ,����ԭ����

		// ray space
		vec3 inv = vec3( 1.0 ) / dir; //ȡ����ʹ�ø�������ı�ֵ������
		vec3 ratio = ( grid + corner - pos ) * inv;//corn+pos��С������
		vec3 ratio_step = grid_step * inv;//����
		//�������rayspaceֻ���ṩһ����ֵ,������grid�Ĳ���ô

		// dda <-��ֵ΢�ַ�
		float hit = -1.0;
		for ( int i = 0; i < 128; i++ ) {
		if ( voxel( grid ) > 0.5 ) {

		hit = 1.0;
		break;   //����Ӧ���ǿ���ֱ���˳�ѭ����,�о�ûʲô����
		continue;
		}

		vec3 cp = step( ratio, ratio.yzx );//��ά����ĸ������,���⻹�о�������ά�ռ��ϵ���չ

		mask = cp * ( vec3( 1.0 ) - cp.zxy );

		grid  += grid_step  * mask;  
		ratio += ratio_step * mask;
		}

		center = grid + vec3( 0.5 );//������ʽ��ʾ(��gridӦ��û�����0 0)
		return dot(ratio - ratio_step,vec3(1.0)) * hit;//dot( ratio - ratio_step, mask ) * hit;
		//������ĵ���hit����Ⱥ���
		*/
		//p1�����
		XMVECTOR start = XMLoadFloat4(&(src.p1));
		XMVECTOR dir = XMVector3Normalize(XMLoadFloat4(&(src.n)));
		XMVECTOR zero = XMVectorSetBinaryConstant(0, 0, 0, 0);
		XMVECTOR one = XMVectorSetBinaryConstant(1, 1, 1, 1);


		XMVECTOR grid;
		XMVECTOR grid_step;
		XMVECTOR grid_corner;
		grid = XMVectorFloor(start);//ʵ����w����Ϊ0Ӧ�þͲ�Ӱ���˰�(
		//����DirectXMathû���ṩSign�ĺ���(��������һ��ͦ��Ť�ķ���- -
		//grid_step ���� sign_dir
		grid_step = DirectX::XMVectorOrInt(DirectX::XMVectorAndInt(dir, DirectX::XMVectorSplatSignMask()), DirectX::XMVectorSplatOne());
		grid_corner = XMVectorClamp(grid_step, zero, one);

		XMVECTOR inv;
		XMVECTOR ratio;
		XMVECTOR ratio_step;
		inv = XMVectorReciprocal(dir);
		ratio = XMVectorMultiply(XMVectorSubtract(XMVectorAdd(grid, grid_corner), start), inv);
		ratio_step = XMVectorMultiply(grid_step, inv);

		bool hit = false;
		XMVECTOR cp;
		XMVECTOR mask;
		XMVECTOR ratioyzx;
		XMVECTOR cpzxy;
		XMFLOAT4 tmp1;
		XMFLOAT4 tmp2;
		int i;
		for (i = 0; i < 128; ++i)//������Ϊ128
		{
			XMStoreFloat4(&tmp1, grid);
			/*
			__try{
			if (map->At(tmp1.x, tmp1.y, tmp1.z).TexType != -1)
			{
			hit = true;
			break;
			}
			}
			__except ((GetExceptionCode() == EXCEPTION_ARRAY_BOUNDS_EXCEEDED)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)
			{
			//����Խ�������Ϊ��������,������û������...
			break;
			}
			*/
			//��������˵�쳣�Ļ��������̫��,�����ж�һ��range��= =
			if ((map->IsLocationInRange(tmp1.x, tmp1.y, tmp1.z)))
			{
				if (map->At(tmp1.x, tmp1.y, tmp1.z).TexType != -1)
				{
					hit = true;
					break;
				}
			}
			/*
			����:����Խ���ʱ�򲢲�һ����Ҫ��ֹ,��Ҫ���ǵ���ֵ�������������.....
			�������㲻breakЧ��Ӧ��Ҳ��ԭ�ȵ��㷨Ҫ��...(����Ҫ����һ������������ⷽ��
			*/


			XMStoreFloat4(&tmp1, ratio);
			tmp2.x = tmp1.y; tmp2.y = tmp1.z; tmp2.z = tmp1.x;
			ratioyzx = XMLoadFloat4(&tmp2);

			cp = XMVectorAndInt(XMVectorGreaterOrEqual(ratioyzx, ratio), XMVectorSplatOne());//1 or 0
			XMStoreFloat4(&tmp1, cp);
			tmp2.x = tmp1.z; tmp2.y = tmp1.x; tmp2.z = tmp1.y;
			cpzxy = XMLoadFloat4(&tmp2);
			mask = XMVectorMultiply(cp, XMVectorSubtract(one, cpzxy));

			grid += XMVectorMultiply(grid_step, mask);
			ratio += XMVectorMultiply(ratio_step, mask);
		}
		if (hit)
		{

			XMFLOAT4 result;
			result = tmp1; //���ڷ���
			if (i == 0)
			{
				//�����ڷ����ڲ������
				result.w = -1;
				return result;
			}
			XMVECTOR ftmp;
			ftmp = XMVectorSubtract(ratio, XMVectorMultiply(ratio_step,mask));
			ftmp = XMVectorAdd(XMVectorScale(dir, XMVectorGetX(DirectX::XMVector3Dot(ftmp, mask))),XMLoadFloat4(&(src.p1)));
			XMStoreFloat4(&tmp1, ftmp);
			//��Ҫȫ��������,��Ϊ�����ʽ��û��ȡ��
			
			result = tmp1;
			result.w = 0;
			return result;
		}
		else
		{
			float tx, ty, tz;
			tz = 0;
			ty = (tz - src.p1.z)*src.n.y / src.n.z + src.p1.y;
			tx = (tz - src.p1.z)*src.n.x / src.n.z + src.p1.x;
			if (ty >= 0 && ty <= (map->GetY()) && tx >= 0 && tx <= (map->GetX()))
			{
				return XMFLOAT4(tx, ty, -1, 0);
			}
		}
		//����ɹ��Ľ������ж�,���Եó�grid�����,������Ҫ��һ���ཻ��(
		{
			XMFLOAT4 result;
			XMVECTOR ftmp;
			ftmp = XMVectorSubtract(ratio, XMVectorMultiply(ratio_step, mask));
			ftmp = XMVectorScale(dir, XMVectorGetX(DirectX::XMVector3Dot(ftmp, mask)));
			XMStoreFloat4(&tmp1, ftmp);
			//��Ҫȫ��������,��Ϊ�����ʽ��û��ȡ��

			result = tmp1;
			return result;
		}
		//return XMFLOAT4(-1, -1, -1, -1);
	}

	DirectX::XMFLOAT4 Scene::TestCollisoin(const LineFunc& src)
	{
		/*
				// grid space
				vec3 grid = floor( pos ); //����ȡ����������������ʹ��
				vec3 grid_step = sign( dir ); //��ȡdir(����)����������<-��˼����˵��ȡ����Ĳ�������,��Ȼֻ������
				vec3 corner = max( grid_step, vec3( 0.0 ) );//����->0 //Ӧ��������ڲ��������ϲ�����ƫ��ֵ,����ԭ����

				// ray space
				vec3 inv = vec3( 1.0 ) / dir; //ȡ����ʹ�ø�������ı�ֵ������
				vec3 ratio = ( grid + corner - pos ) * inv;//corn+pos��С������
				vec3 ratio_step = grid_step * inv;//����
				//�������rayspaceֻ���ṩһ����ֵ,������grid�Ĳ���ô

				// dda <-��ֵ΢�ַ�
				float hit = -1.0;
				for ( int i = 0; i < 128; i++ ) {
				if ( voxel( grid ) > 0.5 ) {
		
				hit = 1.0;
				break;   //����Ӧ���ǿ���ֱ���˳�ѭ����,�о�ûʲô����
				continue;
				}

				vec3 cp = step( ratio, ratio.yzx );//��ά����ĸ������,���⻹�о�������ά�ռ��ϵ���չ

				mask = cp * ( vec3( 1.0 ) - cp.zxy );

				grid  += grid_step  * mask;
				ratio += ratio_step * mask;
				}

				center = grid + vec3( 0.5 );//������ʽ��ʾ(��gridӦ��û�����0 0)
				return dot(ratio - ratio_step,vec3(1.0)) * hit;//dot( ratio - ratio_step, mask ) * hit;
				//������ĵ���hit����Ⱥ���
		*/
		//p1�����
		XMVECTOR start = XMLoadFloat4(&(src.p1));
		XMVECTOR dir = XMVector3Normalize(XMLoadFloat4(&(src.n)));
		XMVECTOR zero = XMVectorSetBinaryConstant(0, 0, 0, 0);
		XMVECTOR one = XMVectorSetBinaryConstant(1, 1, 1, 1);


		XMVECTOR grid;
		XMVECTOR grid_step;
		XMVECTOR grid_corner;
		grid=XMVectorFloor(start);//ʵ����w����Ϊ0Ӧ�þͲ�Ӱ���˰�(
		//����DirectXMathû���ṩSign�ĺ���(��������һ��ͦ��Ť�ķ���- -
		//grid_step ���� sign_dir
		grid_step = DirectX::XMVectorOrInt(DirectX::XMVectorAndInt(dir, DirectX::XMVectorSplatSignMask()), DirectX::XMVectorSplatOne());
		grid_corner = XMVectorClamp(grid_step, zero, one);

		XMVECTOR inv;
		XMVECTOR ratio;
		XMVECTOR ratio_step;
		inv = XMVectorReciprocal(dir);
		ratio = XMVectorMultiply(XMVectorSubtract(XMVectorAdd(grid, grid_corner), start), inv);
		ratio_step = XMVectorMultiply(grid_step, inv);

		bool hit = false;
		XMVECTOR cp;
		XMVECTOR mask;
		XMVECTOR ratioyzx;
		XMVECTOR cpzxy;
		XMFLOAT4 tmp1;
		XMFLOAT4 tmp2;
		int i;
		for (i = 0; i < 128; ++i)//������Ϊ128
		{
			XMStoreFloat4(&tmp1, grid);
			/*
			__try{
				if (map->At(tmp1.x, tmp1.y, tmp1.z).TexType != -1)
				{
					hit = true;
					break;
				}
			}
			__except ((GetExceptionCode() == EXCEPTION_ARRAY_BOUNDS_EXCEEDED)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)
			{
				//����Խ�������Ϊ��������,������û������...
				break;
			}
			*/
			//��������˵�쳣�Ļ��������̫��,�����ж�һ��range��= =
			if ((map->IsLocationInRange(tmp1.x, tmp1.y, tmp1.z)))
			{
				if (map->At(tmp1.x, tmp1.y, tmp1.z).TexType != -1)
				{
					hit = true;
					break;
				}
			}
			/*
				����:����Խ���ʱ�򲢲�һ����Ҫ��ֹ,��Ҫ���ǵ���ֵ�������������.....
				�������㲻breakЧ��Ӧ��Ҳ��ԭ�ȵ��㷨Ҫ��...(����Ҫ����һ������������ⷽ��
			*/
			

			XMStoreFloat4(&tmp1, ratio);
			tmp2.x = tmp1.y; tmp2.y = tmp1.z; tmp2.z = tmp1.x;
			ratioyzx = XMLoadFloat4(&tmp2);
			
			cp = XMVectorAndInt(XMVectorGreaterOrEqual(ratioyzx, ratio), XMVectorSplatOne());//1 or 0
			XMStoreFloat4(&tmp1, cp);
			tmp2.x = tmp1.z; tmp2.y = tmp1.x; tmp2.z = tmp1.y;
			cpzxy = XMLoadFloat4(&tmp2);
			mask = XMVectorMultiply(cp, XMVectorSubtract(one, cpzxy));

			grid += XMVectorMultiply(grid_step, mask);
			ratio += XMVectorMultiply(ratio_step, mask);
		}
		if (hit)
		{
			
			XMFLOAT4 result;
			result = tmp1; //���ڷ���
			if (i == 0)
			{
				//�����ڷ����ڲ������
				result.w = Direction::Up;
				return result;
			}
			XMVECTOR normal;
			normal = XMVectorMultiply(mask, grid_step);
			XMStoreFloat4(&tmp1, normal);
			//��Ҫȫ��������,��Ϊ�����ʽ��û��ȡ��
			if (tmp1.x > 0.5)
			{
				//������Ϊ1,0,0,��
				result.w = Direction::Back;
			}
			else if (tmp1.x < -0.5)
			{
				//������Ϊ-1,0,0,ǰ��
				result.w = Direction::Front;
			}
			else if (tmp1.y < -0.5)
			{
				//������Ϊ0,-1,0,�ҷ�
				result.w = Direction::Right;
			}
			else if (tmp1.y > 0.5)
			{
				//������Ϊ0,1,0,��
				result.w = Direction::Left;
			}
			else if (tmp1.z < -0.5)
			{
				//������Ϊ0,0,-1,�Ϸ�
				result.w = Direction::Up;
			}
			else if (tmp1.z > 0.5)
			{
				//������Ϊ0,0,1,�·�
				result.w = Direction::Down;
			}
			if(false){
				wchar_t test[128];
				XMINT3 n(-tmp1.x, -tmp1.y, -tmp1.z);
				XMINT3 dir(result.x,result.y,result.z);
				wsprintf(test, L"AOMask:%d\n", AOMask(dir, n));
				OutputDebugString(test);
			}
			return result;
		}
		else
		{
			float tx, ty, tz;
		tz = 0;
			ty = (tz - src.p1.z)*src.n.y / src.n.z + src.p1.y;
			tx = (tz - src.p1.z)*src.n.x / src.n.z + src.p1.x;
			if (ty >= 0 && ty <= (map->GetY()) && tx >= 0 && tx <= (map->GetX()))
			{
				return XMFLOAT4((int)tx, (int)ty, -1, Direction::Up);
			}
		}
		//����ɹ��Ľ������ж�,���Եó�grid�����,������Ҫ��һ���ཻ��(
		return XMFLOAT4(-1, -1, -1, -1);
	}

	/*
	DirectX::XMFLOAT4 Scene::TestCollisoin(const LineFunc& src)
	{
		//Note: ʹ�ó�����������,���֮����Ҫ�����Ż����Դ���������
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
	*/
}