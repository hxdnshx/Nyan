#include <list>
#include <functional>
#include <vector>
#include <cassert>
#include <type_traits>
#include <list>

using namespace std;

namespace Nyan
{
	float ddx;
	void* declsample = 0;

	class TimerManage;


	
	//														要变更的值,步长,传递的指针,是否是结束(传进),当前计数
	using timerfunc = std::function < void(const int&, TimerManage*) > ;
	using timerstore = std::pair < timerfunc, int > ;



	class TimerManage
	{
	private:
		struct TimerData
		{
		public:
			int interval;
			int curcnt;
			timerfunc timptr;
			bool isrepect;
			bool flag_remove;
			bool bActive;
			TimerData() = default;
			TimerData(const timerfunc& timp, const int& intv, const bool& bRepeat) : bActive(false), flag_remove(false), curcnt(0), timptr(timp), interval(intv), isrepect(bRepeat)
			{
			}
		};
		static_assert(std::is_standard_layout<TimerData>::value == 1, "TimerData must be POD type");
		vector< int > m_freeslot;
		vector< TimerData > tim;
	private:
		int AllocSlot()
		{
			int ret;
			if (m_freeslot.size() <= 0)
			{
				tim.push_back(TimerData(nullptr, 0, 0));
				ret = tim.size() - 1;
			}
			else
			{
				ret = m_freeslot.back();
				m_freeslot.pop_back();
			}
			return ret;
		}
		void FreeSlot(int i)
		{
			tim[i].timptr = nullptr;
			tim[i].bActive = false;
			tim[i].isrepect = false;
			tim[i].curcnt = 0;
			m_freeslot.push_back(i);
		}
	public:
		using ContainerType = decltype(tim);
		using SelfType = ContainerType::iterator;
		int  SetTimer(timerfunc func, int iInterval, bool bIsRepeat)
		{
			int i = AllocSlot();
			tim[i].timptr = func;
			tim[i].curcnt = 0;
			tim[i].bActive = false;
			tim[i].flag_remove = false;
			tim[i].interval = iInterval;
			tim[i].isrepect = bIsRepeat;
			return i;
		}
		void Tick();
		//

		
		int SetTimer(int inst, timerfunc func, int iInterval, bool bIsRepeat)
		{
			if (func != nullptr)
			{
				tim[inst].timptr = func;
			}
			tim[inst].interval = iInterval;
			tim[inst].isrepect = bIsRepeat;

		}

		/*
			Parameter inst must be an pointer to SelfType(list<TimerData>::iterator)
		*/
		void RemoveTimer(void *inst)
		{
			SelfType* ptr = reinterpret_cast<SelfType*>(inst);
			(*ptr)->flag_remove = true;
		}

		/*
		Parameter inst must be an pointer to SelfType(list<TimerData>::iterator)
		*/
		void StartTimer(void* inst)
		{
			SelfType* ptr = reinterpret_cast<SelfType*>(inst);
			(*ptr)->bActive = true;
		}

		/*
		Parameter inst must be an pointer to SelfType(list<TimerData>::iterator)
		*/
		void PauseTimer(void* inst)
		{
			SelfType* ptr = reinterpret_cast<SelfType*>(inst);
			(*ptr)->bActive = false;
		}

		void Test();
		
	};
}