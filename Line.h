#pragma once

#include<DirectXMath.h>
#include<assert.h>

#define Equal(a, b)  fabs(a-b)<0.0001

namespace Nyan
{
	struct LineFunc
	{
	public:
		typedef DirectX::XMFLOAT4 Point;
		Point p1, p2;
		Point n;//Normal Vector

		LineFunc(float p1x, float p1y,float p1z, float p2x, float p2y,float p2z)
			:p1(p1x, p1y,p1z,0), p2(p2x, p2y,p2z,0)
		{
			float dx, dy,dz;
			dx = p2x - p1x;
			dy = p2y - p1y;
			dz = p2z - p1z;
			n.x = dx;
			n.y = dy;
			n.z = dz;
			n.w = 0;
			//DirectX::XMVECTOR v;
			DirectX::XMStoreFloat4(&n, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&n)));//Normalize
		}

		LineFunc(Point ori, Point dir)
			:p1(ori), n(dir)
		{

		}

		LineFunc(const LineFunc& src)
			:p1(src.p1), p2(src.p2), n(src.n)
		{

		}

		void ReCalc()
		{
			float dx, dy, dz;
			dx = p2.x - p1.x;
			dy = p2.y - p1.y;
			dz = p2.z - p1.z;
			n.x = dx;
			n.y = dy;
			n.z = dz;
			n.w = 0;
			//DirectX::XMVECTOR v;
			DirectX::XMStoreFloat4(&n, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&n)));//Normalize;
		}


		

	};
}