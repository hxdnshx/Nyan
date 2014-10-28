#include "Map.h"
#include "stdio.h"
#include "TCHAR.h"
#include <assert.h>

namespace Nyan
{
	/*
	void Map3D::SetBlock(const int &x, const int &y, const int &z, const int val)
	{
		if (x < 0 || y < 0 || z < 0 || x >= (int)m_layer || y >= (int)m_row || z >= (int)m_col)
		{
			::RaiseException(
				EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
				EXCEPTION_NONCONTINUABLE,
				0, NULL);
		}
		int loc;
		int offset;
		m_block* ptr;
		offset = m_col*m_row*x + m_col*y + z;
		if (m_arr[offset].first == -1)
		{
			if (val != -1)
			{
				loc = AllocateBlock(val);
				ptr = &(m_FastTable[val][loc]);
				(ptr)->TexType = val;
				(ptr)->x = x;
				(ptr)->y = y;
				(ptr)->z = z;
				m_arr[offset].first = val;
				m_arr[offset].second = loc;
			}
		}
		else
		{
			if (val != -1)
			{
				int nloc;
				m_block* nptr;
				nloc = AllocateBlock(val);
				nptr = &(m_FastTable[val][nloc]);
				ptr = &(m_FastTable[m_arr[offset].first][m_arr[offset].second]);
				nptr->TexType = val;
				nptr->mask = (ptr)->mask;
				DeallocateBlock(m_arr[offset].first, m_arr[offset].second);
				(nptr)->x = x;
				(nptr)->y = y;
				(nptr)->z = z;
				m_arr[offset].first = val;
				m_arr[offset].second = nloc;
			}
			else
			{
				DeallocateBlock(m_arr[offset].first, m_arr[offset].second);
			}
		}
	}*/

	void Map3D::SetBlock(const int &x, const int &y, const int &z, const int val)
	{
		if (x < 0 || y < 0 || z < 0 || x >= (int)m_layer || y >= (int)m_row || z >= (int)m_col)
		{
			::RaiseException(
				EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
				EXCEPTION_NONCONTINUABLE,
				0, NULL); /* бл fin бл */
		}
		int loc;
		int offset;
		m_block* ptr;
		offset = m_col*m_row*x + m_col*y + z;
		if (m_arr[offset].first==-1)
		{
			if (val != -1)
			{
				loc = AllocateBlock(val);
				ptr = &(m_FastTable[val][loc]);
				(ptr)->TexType = val;
				(ptr)->x = x;
				(ptr)->y = y;
				(ptr)->z = z;
				m_arr[offset].first = val;
				m_arr[offset].second = loc;
#if defined(Nyan_Map_EnableMaskOptimization)
				BlockCalcMask(x, y, z);
				BlockGetMask(x, y, z);
#endif
			}
		}
		else
		{
			if (val != -1)
			{
				int nloc;
				m_block* nptr;
				nloc = AllocateBlock(val);
				nptr = &(m_FastTable[val][nloc]);
				ptr = &(m_FastTable[m_arr[offset].first][m_arr[offset].second]);
				nptr->TexType = val;
				nptr->mask = (ptr)->mask;
				DeallocateBlock(m_arr[offset].first,m_arr[offset].second);
				(nptr)->x = x;
				(nptr)->y = y;
				(nptr)->z = z;
				m_arr[offset].first = val;
				m_arr[offset].second = nloc;
			}
			else
			{
#if defined(Nyan_Map_EnableMaskOptimization)
				BlockRemoveMask(x, y, z);
#endif
				DeallocateBlock(m_arr[offset].first, m_arr[offset].second);
			}
		}
	}

	int Map3D::AllocateBlock(const int& i)
	{
		int loc;
		m_block* ret;
		if (m_Freeslot[i].GetSize() > 0)
		{
			loc = m_Freeslot[i].Top();
			//ret= &(m_FastTable[i][m_Freeslot[i].Top()]);
			m_Freeslot[i].Pop();
		}
		else
		{
			loc = m_FastTable[i].GetSize();
			m_FastTable[i].Fill(1, -1, -1);
			//m_FastTable[i].FillSimple(1, 0);
			//ret = &(m_FastTable[i].Top());
		}
		return loc;
	}

	void Map3D::DeallocateBlock(const int& val, const int& loc)
	{
		int i = val;
		int j;
		assert(i != -1);
		j = loc;
		assert(j >= 0);
		m_Freeslot[i].Push(j);
		m_FastTable[i][j].TexType = -1;
	}

	void Map3D::SaveToFile(__in const wchar_t* path)
	{
		int i, j;
		FILE* fp;
		int err= _wfopen_s(&fp, path, L"w+");
		if (fp == nullptr)
		{
#ifdef _DEBUG
			assert(false);
#endif
			return;
		}
		for (i = 0; i < m_FastTable.GetSize(); ++i)
		{
			for (j = 0; j < m_FastTable[i].GetSize(); ++j)
			{
				auto obj = &(m_FastTable[i][j]);
				if (obj->TexType != -1)
				{
					fwprintf_s(fp,L"%d %d %d %d \n", obj->x, obj->y, obj->z, obj->TexType);
				}
			}
		}
		fclose(fp);
		return;
	}

	void Map3D::LoadFromFile(__in const wchar_t* path)
	{
		int x, y, z, textype;
		textype = -1;
		FILE* fp;
		_wfopen_s(&fp,path, L"r");
		if (fp == nullptr)
		{
#ifdef _DEBUG
			assert(false);
#endif
			return;
		}
		for (; !feof(fp);)
		{
			if (textype != -1)
			{
				SetBlock(x, y, z, textype);
			}
			_ftscanf_s(fp, L"%d %d %d %d", &x, &y, &z, &textype);
			if (ferror(fp)!=0)
			{
				break;
			}
		}
		fclose(fp);
	}

#if defined(Nyan_Map_EnableMaskOptimization)
	void Map3D::BlockCalcMask(const int& x, const int& y, const int& z)
	{
		emptyblock.mask = -1;
		if (z < (int)m_col - 1		&& (At(x, y, z + 1).mask & Down) == 0)		Get(x, y, z + 1).mask |= Down;
		if (z > 0						&& (At(x, y, z - 1).mask & Up) == 0)			Get(x, y, z - 1).mask |= Up;
		if (y < (int)m_row - 1	&& (At(x, y + 1, z).mask & Left) == 0)		Get(x, y + 1, z).mask |= Left;
		if (y > 0						&& (At(x, y - 1, z).mask & Right) == 0)			Get(x, y - 1, z).mask |= Right;
		if (x < (int)m_layer - 1	&& (At(x + 1, y, z).mask & Back) == 0)			Get(x + 1, y, z).mask |= Back;
		if (x > 0						&& (At(x - 1, y, z).mask & Front) == 0)			Get(x - 1, y, z).mask |= Front;
	}

	void Map3D::BlockRemoveMask(const int& x, const int& y, const int& z)
	{
		emptyblock.mask = 0;
		if (z < (int)m_col - 1		&& (At(x, y, z + 1).mask & Down) == 1)		Get(x, y, z + 1).mask &= 0xffffffff - Down;
		if (z > 0						&& (At(x, y, z - 1).mask & Up) == 1)			Get(x, y, z - 1).mask &= 0xffffffff - Up;
		if (y < (int)m_row - 1	&& (At(x, y + 1, z).mask & Left) == 1)			Get(x, y + 1, z).mask &= 0xffffffff - Left;
		if (y > 0						&& (At(x, y - 1, z).mask & Right) == 1)			Get(x, y - 1, z).mask &= 0xffffffff - Right;
		if (x < (int)m_layer - 1	&& (At(x + 1, y, z).mask & Back) == 1)			Get(x + 1, y, z).mask &= 0xffffffff - Back;
		if (x > 0						&& (At(x - 1, y, z).mask & Front) == 1)			Get(x - 1, y, z).mask &= 0xffffffff - Front;
	}

	void Map3D::BlockGetMask(const int& x, const int& y, const int& z)
	{
		m_block* ptr = &Get(x, y, z);
		ptr->mask = 0;
		emptyblock.TexType = -1;
		if (z < (int)m_col - 1		&& At(x, y, z + 1).TexType != -1)		ptr->mask |= Up;
		if (z > 0						&& At(x, y, z - 1).TexType != -1)		ptr->mask |= Down;
		if (y < (int)m_row - 1	&& At(x, y + 1, z).TexType != -1)		ptr->mask |= Right;
		if (y > 0						&& At(x, y - 1, z).TexType != -1)		ptr->mask |= Left;
		if (x < (int)m_layer - 1	&& At(x + 1, y, z).TexType != -1)		ptr->mask |= Front;
		if (x > 0						&& At(x - 1, y, z).TexType != -1)		ptr->mask |= Back;
		int test = ptr->mask;
	}
#endif

}