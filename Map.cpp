#include "Map.h"
#include <assert.h>

namespace Nyan
{
	void Map3D::SetBlock(const int &x, const int &y, const int &z, const int val)
	{
		if (x < 0 || y < 0 || z < 0 || x >= (int)m_layer || y >= (int)m_row || z >= (int)m_col)
		{
			::RaiseException(
				EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
				EXCEPTION_NONCONTINUABLE,
				0, NULL); /* бл fin бл */
		}
		m_block** ptr;
		if (*(ptr = &(m_arr[m_col*m_row*x + m_col*y + z])) == 0)
		{
			if (val != -1)
			{
				*ptr = AllocateBlock(val);
				(*ptr)->TexType = val;
				BlockCalcMask(x, y, z);
			}
		}
		else
		{
			if (val != -1)
			{
				m_block* nptr;
				nptr = AllocateBlock(val);
				nptr->TexType = (*ptr)->TexType;
				nptr->mask = (*ptr)->mask;
				DeallocateBlock(*ptr);
				*ptr = nptr;
			}
			else
			{
				BlockRemoveMask(x, y, z);
				DeallocateBlock(*ptr);
				*ptr = 0;
			}
		}
	}

	m_block* Map3D::AllocateBlock(const int& i)
	{
		m_block* ret;
		if (m_Freeslot[i].GetSize() > 0)
		{
			ret= &(m_FastTable[i][m_Freeslot[i].Top()]);
			m_Freeslot[i].Pop();
		}
		else
		{
			m_FastTable[i].FillSimple(1, 0);
			ret = &(m_FastTable[i][m_FastTable[i].GetSize() - 1]);
			ret->TexType = -1;
		}
		return ret;
	}

	void Map3D::DeallocateBlock(const m_block* ptr)
	{
		int i = ptr->TexType;
		int j;
		assert(i != -1);
		j = ptr - (m_FastTable[i].GetRaw());
		assert(j >= 0);
		m_Freeslot[i].Push(j);
		m_FastTable[i][j].TexType = -1;
	}

	void Map3D::BlockCalcMask(const int& x, const int& y, const int& z)
	{
		if (z < (int)m_col - 1		&& At(x, y, z + 1).mask || Down == 0)		Get(x, y, z + 1).mask |= Down;
		if (z > 0						&& At(x, y, z - 1).mask || Up == 0)			Get(x, y, z - 1).mask |= Up;
		if (y < (int)m_row - 1	&& At(x, y + 1, z).mask || Right == 0)		Get(x, y + 1, z).mask |= Right;
		if (y > 0						&& At(x, y - 1, z).mask || Left == 0)			Get(x, y - 1, z).mask |= Left;
		if (x < (int)m_layer - 1	&& At(x + 1, y, z).mask || Back == 0)		Get(x + 1, y, z).mask |= Back;
		if (x > 0						&& At(x - 1, y, z).mask || Front == 0)		Get(x - 1, y, z).mask |= Front;
	}

	void Map3D::BlockRemoveMask(const int& x, const int& y, const int& z)
	{
		if (z < (int)m_col - 1		&& At(x, y, z + 1).mask || Down == 1)		Get(x, y, z + 1).mask &= 0xffffffff - Down;
		if (z > 0						&& At(x, y, z - 1).mask || Up == 1)			Get(x, y, z - 1).mask &= 0xffffffff - Up;
		if (y < (int)m_row - 1	&& At(x, y + 1, z).mask || Right == 1)		Get(x, y + 1, z).mask &= 0xffffffff - Right;
		if (y > 0						&& At(x, y - 1, z).mask || Left == 1)			Get(x, y - 1, z).mask &= 0xffffffff - Left;
		if (x < (int)m_layer - 1	&& At(x + 1, y, z).mask || Back == 1)		Get(x + 1, y, z).mask &= 0xffffffff - Back;
		if (x > 0						&& At(x - 1, y, z).mask || Front == 1)		Get(x - 1, y, z).mask &= 0xffffffff - Front;
	}

}