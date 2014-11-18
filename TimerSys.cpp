#include"TimerSys.h"

namespace Nyan
{
	void TimerManage::Tick()
	{
		for (int i = 0; i < tim.size(); ++i)
		{
			if (tim[i].bActive)
			{
				if (++(tim[i].curcnt) >= tim[i].interval)
				{
					tim[i].curcnt = 0;
					if (!tim[i].isrepect)tim[i].bActive = false;
					tim[i].timptr(i, this);
				}
			}
		}
	}

	void TimerManage::Test()
	{
			auto func = std::bind([](const float& arg1, const float& arg2, const int& self, TimerManage* fatherinst)
			{

			}, 1.0f, 2.0f, std::placeholders::_1, std::placeholders::_2);
			SetTimer(func, 1, false);
	}
}