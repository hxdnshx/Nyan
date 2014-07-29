#include "Map.h"

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


}