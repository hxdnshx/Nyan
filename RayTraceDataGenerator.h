#pragma  once

#include "Scene.h"
#include<vector>
#include<functional>

#include "../../../Src/nnnEngine/nnn.h"

using namespace DirectX;

using namespace std;

namespace Nyan
{
	class RayTraceDataGenerator
	{
	public:
		using LocFunc=int(int x, int y, int z);
		using RangeCheckFunc=bool(int x, int y, int z);
		using LocFuncObject = std::function<LocFunc>;
		using RangeCheckFuncObject = std::function<RangeCheckFunc>;
		LocFuncObject GetLocInfo;
		RangeCheckFuncObject RangeCheck;
		RayTraceDataGenerator(decltype(GetLocInfo) LocFunc, decltype(RangeCheck) IsInRange) :GetLocInfo(LocFunc),RangeCheck(IsInRange)
		{
			
		}

		int n2Dir(const XMVECTOR& n);

		bool CalcIntersect(const XMVECTOR& p1, const XMVECTOR& n, XMVECTOR* nResult, XMVECTOR* intersectLocation, float* depth);

		void GenerateRaytraceResult(int width, int height,float scale, XMFLOAT4 loc, int neardistance, int maxdepth, XMFLOAT4 n, XMFLOAT4 up, void(*CallbackFunc)(int x, int y, const int& TexType, const XMFLOAT4& loc, const XMFLOAT4& n,const float& depth, void* arg), void* arg);

	};
}