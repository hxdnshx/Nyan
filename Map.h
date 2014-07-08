#include "MinimalAllocator.hpp"

#define MINIMAL_USE_PROCESSHEAPSTRING
#include "MinimalPath.hpp"

#define MINIMAL_USE_PROCESSHEAPARRAY
#include "MinimalArray.hpp"


namespace Nyan
{
	enum Direction{
		Up = 1,
		Down = Up<<1,
		Left=Down<<1,
		Right=Left<<1,
		Front=Right<<1,
		Back=Front<<1,
		Direction_MAX=Back<<1
	};
	struct m_rect
	{
		int Texid;
	};
	struct m_block
	{
		typedef m_block SelfType;
		int TexType;
		int mask;
		m_block()
		{
			TexType = -1;
			mask = 0;
		}
		m_block(const SelfType& src)
		{
			TexType = src.TexType;
			mask = src.mask;
		}
		m_block(int m_TexType, int m_mask)
		{
			TexType = m_TexType;
			mask = m_mask;
		}
	};
	
	class Map3D : protected Minimal::MinimalArrayT<m_block>
	{
	public:
		typedef Minimal::MinimalArrayT < BYTE >  SaveFormat;
	protected:
		size_t m_layer;//x
		size_t m_row;//y
		size_t m_col;//z

		int m_tcnt;//TextureCount
		//void BuildVertexBuffer();//其实不应该把VertexBuffer写到Map中
	public:
		MinimalArrayT< MinimalArrayT< m_block* > > m_FastTable;//为了加快检索效率的表
		int CalcMask();
		void ClearMask();
		void ReCalcMask();
		void ReCalcBlock(const int&, const int&, const int&);

		void OutBinary(__in bool isSaveMask, __out SaveFormat& bin);

		
		inline size_t GetX(){ return m_layer; }
		inline size_t GetY(){ return m_row; }
		inline size_t GetZ(){ return m_col; }

		//获取纹理总数
		inline size_t GetT(){ return m_tcnt; }

		void SetT(size_t cnt)
		{
			if (cnt < m_tcnt)
			{
				::RaiseException(
					EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
					EXCEPTION_NONCONTINUABLE,
					0, NULL); /* ～ fin ～ */
			}
			m_FastTable.Fill(cnt - m_tcnt,m_alloc);
		}

		//实际存储方式为x-y-z形式(z层面上是连续存储的
		inline m_block& At(const int &x,const int &y,const int &z)
		{
			if (x < 0 || y < 0 || z < 0 || x >= m_layer || y >= m_row || z >= m_col)
			{
				::RaiseException(
					EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
					EXCEPTION_NONCONTINUABLE,
					0, NULL); /* ～ fin ～ */
			}
			return m_arr[m_layer*m_row*x + m_row*y + z];
		}
		
		Map3D(__in Minimal::IMinimalAllocator *alloc, __in SaveFormat& bin);

		Map3D(Minimal::IMinimalAllocator *alloc, const int& layer, const int& row, const int& col) :
			MinimalArrayT<m_block>(layer*row*col, alloc), m_layer(layer), m_row(row), m_col(col), m_tcnt(0), m_FastTable(alloc)
		{}
	};
	
}