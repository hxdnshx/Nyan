#include"TimerSys.h"

namespace Nyan
{
	void TimerManage::Tick()
	{
		tim.remove_if([](const TimerData& dat) -> bool
		{
			return dat.flag_remove;
		});
		for (auto iter = tim.begin(); iter != tim.end(); ++iter)
		{
			if (iter->bActive)
			{
				if ((iter->curcnt)++ >= iter->interval)
				{
					iter->curcnt = 0;
					if (!iter->isrepect) iter->bActive = false;
					iter->timptr(static_cast<void*>(&(*iter)), this);
					

				}
			}
		}
	}

	void TimerManage::Test()
	{
			auto func = std::bind([](const float& arg1, const float& arg2, void* self, TimerManage* fatherinst)
			{

			}, 1.0f, 2.0f, std::placeholders::_1, std::placeholders::_2);
			SetTimer(func, 1, false);
	}
}