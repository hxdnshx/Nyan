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
			//������Ϊ1,0,0,��
			ret = Direction::Back;
		}
		else if (tmp1.x < -0.5)
		{
			//������Ϊ-1,0,0,ǰ��
			ret = Direction::Front;
		}
		else if (tmp1.y < -0.5)
		{
			//������Ϊ0,-1,0,�ҷ�
			ret = Direction::Right;
		}
		else if (tmp1.y > 0.5)
		{
			//������Ϊ0,1,0,��
			ret = Direction::Left;
		}
		else if (tmp1.z < -0.5)
		{
			//������Ϊ0,0,-1,�Ϸ�
			ret = Direction::Up;
		}
		else if (tmp1.z > 0.5)
		{
			//������Ϊ0,0,1,�·�
			ret = Direction::Down;
		}
		return ret;
	}

	bool RayTraceDataGenerator::CalcIntersect(const XMVECTOR& p1, const XMVECTOR& n, XMVECTOR* nResult, XMVECTOR* intersectLocation, float* depth)
	{
		/*
		// grid space
		vec3 grid = floor( pos ); //����ȡ����������������ʹ��
		vec3 grid_step = sign( dir ); //��ȡdir(����)����������<-��˼����˵��ȡ����Ĳ�������,��Ȼֻ������
		vec3 corner = max( grid_step, vec3( 0.0 ) );//����->0 //Ӧ��������ڲ��������ϲ�����ƫ��ֵ,����ԭ����

		// ray space
		vec3 inv = vec3( 1.0 ) / dir; //ȡ����ʹ�ø�������ı�ֵ������
		vec3 ratio = ( grid + corner - pos ) * inv;//corn+pos��С������
		vec3 ratio_step = grid_step * inv;//����
		//�������rayspaceֻ���ṩһ����ֵ,������grid�Ĳ���ô

		// dda <-��ֵ΢�ַ�
		float hit = -1.0;
		for ( int i = 0; i < 128; i++ ) {
		if ( voxel( grid ) > 0.5 ) {

		hit = 1.0;
		break;   //����Ӧ���ǿ���ֱ���˳�ѭ����,�о�ûʲô����
		continue;
		}

		vec3 cp = step( ratio, ratio.yzx );//��ά����ĸ������,���⻹�о�������ά�ռ��ϵ���չ

		mask = cp * ( vec3( 1.0 ) - cp.zxy );

		grid  += grid_step  * mask;
		ratio += ratio_step * mask;
		}

		center = grid + vec3( 0.5 );//������ʽ��ʾ(��gridӦ��û�����0 0)
		return dot(ratio - ratio_step,vec3(1.0)) * hit;//dot( ratio - ratio_step, mask ) * hit;
		//������ĵ���hit����Ⱥ���
		*/
		//p1�����
		XMVECTOR start = p1;
		XMVECTOR dir = n;
		XMVECTOR zero = XMVectorSetBinaryConstant(0, 0, 0, 0);
		XMVECTOR one = XMVectorSetBinaryConstant(1, 1, 1, 1);


		XMVECTOR grid;
		XMVECTOR grid_step;
		XMVECTOR grid_corner;
		grid = XMVectorFloor(start);//ʵ����w����Ϊ0Ӧ�þͲ�Ӱ���˰�(
		//����DirectXMathû���ṩSign�ĺ���(��������һ��ͦ��Ť�ķ���- -
		//grid_step ���� sign_dir
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
		for (i = 0; i < 128; ++i)//������Ϊ128
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
			//����Խ�������Ϊ��������,������û������...
			break;
			}
			*/
			//��������˵�쳣�Ļ��������̫��,�����ж�һ��range��= =
			if ((RangeCheck(tmp1.x, tmp1.y, tmp1.z)))
			{
				if (GetLocInfo(tmp1.x, tmp1.y, tmp1.z) != -1)
				{
					hit = true;
					break;
				}
			}
			/*
			����:����Խ���ʱ�򲢲�һ����Ҫ��ֹ,��Ҫ���ǵ���ֵ�������������.....
			�������㲻breakЧ��Ӧ��Ҳ��ԭ�ȵ��㷨Ҫ��...(����Ҫ����һ������������ⷽ��
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
			result = tmp1; //���ڷ���
			if (i == 0)
			{
				//�����ڷ����ڲ������
				result.w = -1;
				return false;
			}
			XMVECTOR ftmp;
			ftmp = XMVectorSubtract(ratio, ratio_step);//��Ϊȡ��ֻ��mask�����ֵ,����step������û��Ҫ��mask
			*depth = XMVectorGetX(DirectX::XMVector3Dot(ftmp, mask));
			ftmp = XMVectorAdd(XMVectorScale(dir,*depth) , p1);
			XMStoreFloat4(&tmp1, ftmp);
			//��Ҫȫ��������,��Ϊ�����ʽ��û��ȡ��
			*intersectLocation = ftmp;
			result = tmp1;
			result.w = 0;

			XMVECTOR normal;
			normal = XMVectorMultiply(mask, grid_step);
			XMStoreFloat4(&tmp1, normal);
			//��Ҫȫ��������,��Ϊ�����ʽ��û��ȡ��
			if (tmp1.x > 0.5)
			{
				//������Ϊ1,0,0,��
				*nResult = XMVectorSetBinaryConstant(1, 0, 0, 0);
			}
			else if (tmp1.x < -0.5)
			{
				//������Ϊ-1,0,0,ǰ��
				*nResult = XMVectorSetBinaryConstant(-1, 0, 0, 0);
			}
			else if (tmp1.y < -0.5)
			{
				//������Ϊ0,-1,0,�ҷ�
				*nResult = XMVectorSetBinaryConstant(0, -1, 0, 0);
			}
			else if (tmp1.y > 0.5)
			{
				//������Ϊ0,1,0,��
				*nResult = XMVectorSetBinaryConstant(0, 1, 0, 0);
			}
			else if (tmp1.z < -0.5)
			{
				//������Ϊ0,0,-1,�Ϸ�
				*nResult = XMVectorSetBinaryConstant(0, 0, -1, 0);
			}
			else if (tmp1.z > 0.5)
			{
				//������Ϊ0,0,1,�·�
				*nResult = XMVectorSetBinaryConstant(0, 0, 1, 0);
			}
			return true;
			//return result;
		}
		else
		{
			return false;
		}
		//����ɹ��Ľ������ж�,���Եó�grid�����,������Ҫ��һ���ཻ��(
		return false;
	}
}