#pragma once

#include "RayTraceDataGenerator.h"
#include "Scene.h"
#include<functional>

using namespace std;
using namespace std::placeholders;

namespace Nyan
{
	class Scene2RayAdapter
	{
	public:
		Scene2RayAdapter(Scene* source) :target(nullptr)
		{
			auto Func_GetBlock = [](Scene* source, int x, int y, int z) ->int
			{
				return source->GetMap()->GetBlock(x, y, z).TexType;
			};
			auto Func_RangeCheck = [](Scene *source, int x, int y, int z) -> bool
			{
				return source->GetMap()->IsLocationInRange(x, y, z);
			};
			auto bind_GetBlock = std::bind(Func_GetBlock, source, _1,_2,_3);
			auto bind_RangeCheck = std::bind(Func_RangeCheck, source, _1,_2,_3);
			//bind_GetBlock(1, 2, 3);
			//bind_RangeCheck(1, 2, 3);
			target = new RayTraceDataGenerator(bind_GetBlock, bind_RangeCheck);
		}

		RayTraceDataGenerator& operator ()()
		{
			return *target;
		}

		~Scene2RayAdapter()
		{
			if (target != nullptr)
			{
				delete target;
			}
		}
		RayTraceDataGenerator *target;
	};
}