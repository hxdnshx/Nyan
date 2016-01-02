#include "RayTraceDataGenerator.h"

namespace Nyan
{
	void RayTraceDataGenerator::GenerateRaytraceResult(int width, int height,float scale, XMFLOAT4 loc, int neardistance, int maxdepth, XMFLOAT4 n, XMFLOAT4 up, void(*CallbackFunc)(int x, int y, const int& TexType, const XMFLOAT4& loc, const XMFLOAT4& n,const float& depth, void* arg), void* arg)
	{
		XMVECTOR v_ori = XMLoadFloat4(&loc);
		XMVECTOR v_center=XMLoadFloat4(&loc);
		XMVECTOR v_n = XMLoadFloat4(&n);
		XMVECTOR v_up = XMVector3Normalize(XMLoadFloat4(&up));
		v_center = XMVectorAdd(v_center, XMVectorScale(v_n, neardistance));
		XMVECTOR v_dir_x = XMVector3Cross(v_n, v_up);
		XMVECTOR v_dir_y = XMVector3Cross(v_n, v_dir_x);
		XMVECTOR v_current = XMVectorAdd(v_center, XMVectorScale(v_dir_x, -scale*width*0.5));
		XMVECTOR v_ret_n;
		XMVECTOR v_ret_loc;
		//XMFLOAT4 v_unpacked;
		XMFLOAT4 v_ret_unpack_n;
		XMFLOAT4 v_ret_unpack_loc;
		XMVECTOR v_corner1;
		XMVECTOR v_corner2;
		bool result;
		float f_ret_depth;
		v_current = XMVectorAdd(v_current, XMVectorScale(v_dir_y, -scale*height*0.5));
		v_corner1 = v_current;
		v_corner2 = XMVectorAdd(v_current, XMVectorScale(v_dir_y, +scale*height*1.0));
		v_corner2 = XMVectorAdd(v_corner2, XMVectorScale(v_dir_x, +scale*width*1.0));
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{//XMVector3Normalize(XMVectorSubtract(v_current, v_ori))
				result = CalcIntersect(v_current, v_n, &v_ret_n, &v_ret_loc, &f_ret_depth);
				if (result)
				{
					XMStoreFloat4(&v_ret_unpack_loc,v_ret_loc);
					XMStoreFloat4(&v_ret_unpack_n, v_ret_n);
					CallbackFunc(i, j, GetLocInfo((int)v_ret_unpack_loc.x, (int)v_ret_unpack_loc.y, (int)v_ret_unpack_loc.z), v_ret_unpack_loc, v_ret_unpack_n, f_ret_depth, arg);
				}
				else
				{
					v_ret_unpack_n.x = v_ret_unpack_n.y = v_ret_unpack_n.z = -1;
					v_ret_unpack_loc.x = v_ret_unpack_loc.y = v_ret_unpack_loc.z = -1;
					f_ret_depth = -1;
					CallbackFunc(i, j, -1, v_ret_unpack_loc, v_ret_unpack_n, f_ret_depth, arg);
				}
				v_current = XMVectorAdd(v_current, XMVectorScale(v_dir_y, scale));
			}
			v_current = XMVectorAdd(v_current, XMVectorScale(v_dir_x, scale));
		}
	}

	int RayTraceDataGenerator::n2Dir(const XMVECTOR& n)
	{
		XMFLOAT4 tmp1;
		int ret;
		XMStoreFloat4(&tmp1, n);
		if (tmp1.x > 0.5)
		{
			//法向量为1,0,0,后方
			ret = Direction::Back;
		}
		else if (tmp1.x < -0.5)
		{
			//法向量为-1,0,0,前方
			ret = Direction::Front;
		}
		else if (tmp1.y < -0.5)
		{
			//法向量为0,-1,0,右方
			ret = Direction::Right;
		}
		else if (tmp1.y > 0.5)
		{
			//法向量为0,1,0,左方
			ret = Direction::Left;
		}
		else if (tmp1.z < -0.5)
		{
			//法向量为0,0,-1,上方
			ret = Direction::Up;
		}
		else if (tmp1.z > 0.5)
		{
			//法向量为0,0,1,下方
			ret = Direction::Down;
		}
		return ret;
	}

	bool RayTraceDataGenerator::CalcIntersect(const XMVECTOR& p1, const XMVECTOR& n, XMVECTOR* nResult, XMVECTOR* intersectLocation, float* depth)
	{
		/*
		// grid space
		vec3 grid = floor( pos ); //向下取整将坐标在网格中使用
		vec3 grid_step = sign( dir ); //获取dir(方向)的正负符号<-意思就是说获取网格的步进方向,虽然只是正负
		vec3 corner = max( grid_step, vec3( 0.0 ) );//负号->0 //应该是最后在步进方向上产生的偏差值,但是原因不明

		// ray space
		vec3 inv = vec3( 1.0 ) / dir; //取倒数使得各个方向的比值倒过来
		vec3 ratio = ( grid + corner - pos ) * inv;//corn+pos的小数部分
		vec3 ratio_step = grid_step * inv;//不懂
		//于是这个rayspace只是提供一个比值,来决定grid的步进么

		// dda <-数值微分法
		float hit = -1.0;
		for ( int i = 0; i < 128; i++ ) {
		if ( voxel( grid ) > 0.5 ) {

		hit = 1.0;
		break;   //这里应该是可以直接退出循环的,感觉没什么区别
		continue;
		}

		vec3 cp = step( ratio, ratio.yzx );//二维情况的搞清楚了,问题还有就是在三维空间上的扩展

		mask = cp * ( vec3( 1.0 ) - cp.zxy );

		grid  += grid_step  * mask;
		ratio += ratio_step * mask;
		}

		center = grid + vec3( 0.5 );//中心形式表示(跟grid应该没区别吧0 0)
		return dot(ratio - ratio_step,vec3(1.0)) * hit;//dot( ratio - ratio_step, mask ) * hit;
		//这里关心的是hit的深度好像
		*/
		//p1是起点
		XMVECTOR start = p1;
		XMVECTOR dir = n;
		XMVECTOR zero = XMVectorSetBinaryConstant(0, 0, 0, 0);
		XMVECTOR one = XMVectorSetBinaryConstant(1, 1, 1, 1);


		XMVECTOR grid;
		XMVECTOR grid_step;
		XMVECTOR grid_corner;
		grid = XMVectorFloor(start);//实际上w分量为0应该就不影响了吧(
		//好像DirectXMath没有提供Sign的函数(于是用了一个挺别扭的方法- -
		//grid_step 就是 sign_dir
		grid_step = DirectX::XMVectorOrInt(DirectX::XMVectorAndInt(dir, DirectX::XMVectorSplatSignMask()), DirectX::XMVectorSplatOne());
		grid_corner = XMVectorClamp(grid_step, zero, one);

		XMVECTOR inv;
		XMVECTOR ratio;
		XMVECTOR ratio_step;
		inv = XMVectorReciprocal(dir);
		ratio = XMVectorMultiply(XMVectorSubtract(XMVectorAdd(grid, grid_corner), start), inv);
		ratio_step = XMVectorMultiply(grid_step, inv);

		bool hit = false;
		XMVECTOR cp;
		XMVECTOR mask;
		XMVECTOR ratioyzx;
		XMVECTOR cpzxy;
		XMFLOAT4 tmp1;
		XMFLOAT4 tmp2;
		int i;
		for (i = 0; i < 128; ++i)//最大深度为128
		{
			XMStoreFloat4(&tmp1, grid);
			/*
			__try{
			if (map->At(tmp1.x, tmp1.y, tmp1.z).TexType != -1)
			{
			hit = true;
			break;
			}
			}
			__except ((GetExceptionCode() == EXCEPTION_ARRAY_BOUNDS_EXCEEDED)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)
			{
			//捕获越界错误作为跳出条件,看看有没有问题...
			break;
			}
			*/
			//理论上来说异常的话处理代价太大,还是判断一下range吧= =
			if ((RangeCheck(tmp1.x, tmp1.y, tmp1.z)))
			{
				if (GetLocInfo(tmp1.x, tmp1.y, tmp1.z) != -1)
				{
					hit = true;
					break;
				}
			}
			/*
			修正:发生越界的时候并不一定就要终止,需要考虑到从值域外射出的射线.....
			不过就算不break效率应该也比原先的算法要高...(除了要限制一下最大遍历深度这方面
			*/


			XMStoreFloat4(&tmp1, ratio);
			tmp2.x = tmp1.y; tmp2.y = tmp1.z; tmp2.z = tmp1.x;
			ratioyzx = XMLoadFloat4(&tmp2);

			cp = XMVectorAndInt(XMVectorGreaterOrEqual(ratioyzx, ratio), XMVectorSplatOne());//1 or 0
			XMStoreFloat4(&tmp1, cp);
			tmp2.x = tmp1.z; tmp2.y = tmp1.x; tmp2.z = tmp1.y;
			cpzxy = XMLoadFloat4(&tmp2);
			mask = XMVectorMultiply(cp, XMVectorSubtract(one, cpzxy));

			grid += XMVectorMultiply(grid_step, mask);
			ratio += XMVectorMultiply(ratio_step, mask);
		}
		if (hit)
		{

			XMFLOAT4 result;
			result = tmp1; //所在方块
			if (i == 0)
			{
				//这是在方块内部的情况
				result.w = -1;
				return false;
			}
			XMVECTOR ftmp;
			ftmp = XMVectorSubtract(ratio, ratio_step);//因为取的只是mask方向的值,所以step在这里没必要乘mask
			*depth = XMVectorGetX(DirectX::XMVector3Dot(ftmp, mask));
			ftmp = XMVectorAdd(XMVectorScale(dir,*depth) , p1);
			XMStoreFloat4(&tmp1, ftmp);
			//需要全部反过来,因为上面的式子没有取反
			*intersectLocation = ftmp;
			result = tmp1;
			result.w = 0;

			XMVECTOR normal;
			normal = XMVectorMultiply(mask, grid_step);
			XMStoreFloat4(&tmp1, normal);
			//需要全部反过来,因为上面的式子没有取反
			if (tmp1.x > 0.5)
			{
				//法向量为1,0,0,后方
				*nResult = XMVectorSetBinaryConstant(1, 0, 0, 0);
			}
			else if (tmp1.x < -0.5)
			{
				//法向量为-1,0,0,前方
				*nResult = XMVectorSetBinaryConstant(-1, 0, 0, 0);
			}
			else if (tmp1.y < -0.5)
			{
				//法向量为0,-1,0,右方
				*nResult = XMVectorSetBinaryConstant(0, -1, 0, 0);
			}
			else if (tmp1.y > 0.5)
			{
				//法向量为0,1,0,左方
				*nResult = XMVectorSetBinaryConstant(0, 1, 0, 0);
			}
			else if (tmp1.z < -0.5)
			{
				//法向量为0,0,-1,上方
				*nResult = XMVectorSetBinaryConstant(0, 0, -1, 0);
			}
			else if (tmp1.z > 0.5)
			{
				//法向量为0,0,1,下方
				*nResult = XMVectorSetBinaryConstant(0, 0, 1, 0);
			}
			return true;
			//return result;
		}
		else
		{
			return false;
		}
		//上面成功的进行了判断,可以得出grid编号了,不过还要算一下相交面(
		return false;
	}
}