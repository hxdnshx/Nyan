#include "Map.h"

using namespace Nyan;

void Map3D::ReCalcMask()
{
	ClearMask();
	CalcMask();
}

int Map3D::CalcMask()
{
	int x, y, z,cnt=0,cnt2=0;
	for (x = 0; x < m_tcnt; ++x)
	{
		m_FastTable[x].Clear();
	}
	for (x = 0; x < (int)m_layer; ++x)
	{
		for (y = 0; y < (int)m_row; ++y)
		{
			for (z = 0; z < (int)m_col; ++z)
			{
				if (At(x, y, z).TexType != -1)
				{
					if (z < (int)m_col - 1)		(At(x, y, z + 1).mask |= Down);// ++cnt;
					if (z > 0)					(At(x, y, z - 1).mask |= Up);// ++cnt;
					if (y <(int)m_row - 1)		(At(x, y + 1, z).mask |= Left);// ++cnt;
					if (y > 0)					(At(x, y - 1, z).mask |= Right);// ++cnt;
					if (x <(int)m_layer - 1)	(At(x + 1, y, z).mask |= Back);// ++cnt;
					if (x > 0)					(At(x - 1, y, z).mask |= Front);// ++cnt;
					if (At(x, y, z).TexType < m_tcnt && At(x, y, z).TexType>=0)
					{
						m_FastTable[At(x, y, z).TexType].Push(&At(x, y, z));
					}
				}
			}
		}
	}
	return cnt;
}

int Map3D::CountRect()
{
	int x, y, z, cnt = 0;
	for (x = 0; x < (int)m_layer; ++x)
	{
		for (y = 0; y < (int)m_row; ++y)
		{
			for (z = 0; z < (int)m_col; ++z)
			{
				if (At(x, y, z).TexType != -1)
				{
					cnt += 6;
					if ((At(x, y, z).mask & Nyan::Up) != 0)cnt--;
					if ((At(x, y, z).mask & Nyan::Down) != 0)cnt--;
					if ((At(x, y, z).mask & Nyan::Left) != 0)cnt--;
					if ((At(x, y, z).mask & Nyan::Right) != 0)cnt--;
					if ((At(x, y, z).mask & Nyan::Front) != 0)cnt--;
					if ((At(x, y, z).mask & Nyan::Back) != 0)cnt--;
				}
			}
		}
	}
	return cnt;
}

void Map3D::ClearMask()
{
	int x, y, z;
	for (x = 0; x < (int)m_layer; ++x)
	{
		for (y = 0; y < (int)m_row; ++y)
		{
			for (z = 0; z < (int)m_col; ++z)
			{
				if (At(x, y, z).TexType != -1)
				{
					At(x, y, z).mask = 0;
				}
			}
		}
	}
}

void Map3D::ReCalcBlock(const int& x, const int& y, const int& z)
{
	if (z < (int)m_col - 1)		At(x, y, z + 1).mask |= Down;
	if (z > 0)					At(x, y, z - 1).mask |= Up;
	if (y < (int)m_row - 1)		At(x, y + 1, z).mask |= Right;
	if (y > 0)					At(x, y - 1, z).mask |= Left;
	if (x < (int)m_layer - 1)	At(x + 1, y, z).mask |= Back;
	if (x > 0)					At(x - 1, y, z).mask |= Front;
}

void Map3D::OutBinary(__in bool isSaveMask, __out SaveFormat& bin)
{
	size_t b_size;
	WORD* wordptr;
	b_size = 128 + m_layer*m_row*m_col*4;//128Byte For Info
	bin.Grow(b_size);
	wordptr = (WORD*)bin.GetRaw();
	bin[0] = 0x2b;//
	bin[1] = 0x45;//前置...嗯
	bin[2] = 0x01;//文件版本:Ver.1
	bin[3] = isSaveMask?0x1:0;
	int i = 128;
	wordptr[2] = (WORD)m_layer;
	wordptr[3] = (WORD)m_row;
	wordptr[4] = (WORD)m_col;

	//实际数据写入
	int x, y, z;
	for (x = 0; x < (int)m_layer; ++x)
	{
		for (y = 0; y < (int)m_row; ++y)
		{
			for (z = 0; z < (int)m_col; ++z)
			{
				if (At(x, y, z).TexType != -1)
				{
					bin[++i] = (unsigned char)At(x, y, z).TexType;
					bin[++i] = isSaveMask?(unsigned char)At(x, y, z).mask:0xff;
					bin[++i] = 0xff;
					bin[++i] = 0xff;
				}
				else
				{
					bin[++i] = 0xff;
					bin[++i] = 0xff;
					bin[++i] = 0xff;
					bin[++i] = 0xff;
				}
			}
		}
	}
}

Map3D::Map3D(__in Minimal::IMinimalAllocator *alloc, __in SaveFormat& bin):
Minimal::MinimalArrayT<m_block>(alloc), m_FastTable(alloc), m_tcnt(0)
{
	if (bin[0] != 0x2b || bin[1] != 0x54)
	{
		::RaiseException(
			EXCEPTION_READ_FAULT,
			EXCEPTION_NONCONTINUABLE,
			0, NULL); /* ～ fin ～ */
	}
	switch (bin[2]) //Version
	{
	case 0x01:
		{
			WORD* wordptr;
			bool isSavedMask;
			int x, y, z;
			int i = 128;
			isSavedMask = (bin[3] == 0x1);
			wordptr = (WORD*)bin.GetRaw();
			m_layer = wordptr[2];
			m_row = wordptr[3];
			m_col = wordptr[4];
			if (m_layer == 0 || m_row == 0 || m_col == 0)
			{
				::RaiseException(
					EXCEPTION_READ_FAULT,
					EXCEPTION_NONCONTINUABLE,
					0, NULL); /* ～ fin ～ */
			}
			for (x = 0; x < (int)m_layer; ++x)
			{
				for (y = 0; y < (int)m_row; ++y)
				{
					for (z = 0; z < (int)m_col; ++z)
					{
						At(x, y, z).TexType = bin[++i];
						if (At(x, y, z).TexType == 0xff)
						{
							At(x, y, z).TexType = -1;
							++i;
						}
						else
						{
							At(x, y, z).mask = isSavedMask ? bin[++i] : 0;
						}
						++i;
						++i;
					}
				}
			}
			if (!isSavedMask)
			{
				CalcMask();
			}
		}
		break;
	default:
		::RaiseException(
			EXCEPTION_READ_FAULT,
			EXCEPTION_NONCONTINUABLE,
			0, NULL); /* ～ fin ～ */
	}
}

	//关于渲染效率的提高,一方面是mask,也可以通过输入几个平面,对渲染内容进行限定