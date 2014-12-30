#include "SelectRect.h"
#include "Map.h"

#define scale 1.0

namespace Nyan
{
	DirectX::XMFLOAT2 SelectRect::GetTexloc(int loc)
	{
		return DirectX::XMFLOAT2((loc & 0x1) > 0 ? umax:umin, (loc & 0x2) > 0 ? vmax : vmin);
	}
	void SelectRect::SetRectLocation(const int& nx, const int& ny, const int& nz, const int& dir)
	{
		Minimal::MinimalArrayT< VertexType > m_vlist(m_alloc);
		Minimal::MinimalArrayT< WORD > m_ilist(m_alloc);
		VertexType vert;
		if (x == nx && y == ny && z == nz && dir == loc)
		{
			return;
		}
		int itmp;
		x = nx;
		int py = y = ny;
		int pz = z = nz;
		loc = dir;
		m_vlist.Grow(4);
		m_ilist.Grow(6);
		//vert.m_Color = 0x883333ff;
		//vert.UpdateData();
		vert.m_Normal = DirectX::XMFLOAT3(0, 0, 0);
		vert.aomask = 0;
		if (dir != -1)
		{
			if ((dir & (Nyan::Down)) != 0)
			{
				itmp = m_vlist.GetSize();
				//vert.m_Color = 0xffffffff;
				//vert.m_Color_dx11_opengl = 0xffffffff;
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 0)*scale - 0.1);
				vert.m_Tex = GetTexloc( 0);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 0)*scale - 0.1);
				vert.m_Tex = GetTexloc( 1);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 0)*scale - 0.1);
				vert.m_Tex = GetTexloc( 2);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 0)*scale - 0.1);
				vert.m_Tex = GetTexloc( 3);
				m_vlist.Push(vert);
				m_ilist.Push((WORD)itmp + 3);
				m_ilist.Push((WORD)itmp + 1);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 2);
				m_ilist.Push((WORD)itmp + 3);
			}
			//continue;

			if ((dir & (Nyan::Up)) != 0)
			{
				itmp = m_vlist.GetSize();
				//vert.m_Color = 0xffffffff;
				//vert.m_Color_dx11_opengl = 0xffffffff;
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale, (pz + 1)*scale + 0.1);
				vert.m_Tex = GetTexloc( 0);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale, (pz + 1)*scale + 0.1);
				vert.m_Tex = GetTexloc( 1);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale, (pz + 1)*scale + 0.1);
				vert.m_Tex = GetTexloc( 2);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale, (pz + 1)*scale + 0.1);
				vert.m_Tex = GetTexloc( 3);
				m_vlist.Push(vert);
				m_ilist.Push((WORD)itmp + 3);
				m_ilist.Push((WORD)itmp + 1);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 2);
				m_ilist.Push((WORD)itmp + 3);
			}

			//continue;

			if ((dir & (Nyan::Left)) != 0)
			{
				itmp = m_vlist.GetSize();
				//vert.m_Color = 0xffffffff;
				//vert.m_Color_dx11_opengl = 0xffffffff;
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale - 0.1, (pz + 1)*scale);
				vert.m_Tex = GetTexloc( 0);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale - 0.1, (pz + 1)*scale);
				vert.m_Tex = GetTexloc( 1);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 0)*scale - 0.1, (pz + 0)*scale);
				vert.m_Tex = GetTexloc( 2);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 0)*scale - 0.1, (pz + 0)*scale);
				vert.m_Tex = GetTexloc( 3);
				m_vlist.Push(vert);
				m_ilist.Push((WORD)itmp + 3);
				m_ilist.Push((WORD)itmp + 1);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 2);
				m_ilist.Push((WORD)itmp + 3);
			}

			//continue;

			if ((dir & (Nyan::Right)) != 0)
			{
				itmp = m_vlist.GetSize();
				//vert.m_Color = 0xffffffff;
				//vert.m_Color_dx11_opengl = 0xffffffff;
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale + 0.1, (pz + 1)*scale);
				vert.m_Tex = GetTexloc( 0);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale + 0.1, (pz + 1)*scale);
				vert.m_Tex = GetTexloc( 1);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale, (py + 1)*scale + 0.1, (pz + 0)*scale);
				vert.m_Tex = GetTexloc( 2);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale, (py + 1)*scale + 0.1, (pz + 0)*scale);
				vert.m_Tex = GetTexloc( 3);
				m_vlist.Push(vert);
				m_ilist.Push((WORD)itmp + 3);
				m_ilist.Push((WORD)itmp + 1);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 2);
				m_ilist.Push((WORD)itmp + 3);
			}



			if ((dir & (Nyan::Front)) != 0)
			{
				itmp = m_vlist.GetSize();
				//vert.m_Color = 0xffffffff;
				//vert.m_Color_dx11_opengl = 0xffffffff;
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale + 0.1, (py + 0)*scale, (pz + 1)*scale);
				vert.m_Tex = GetTexloc( 0);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale + 0.1, (py + 1)*scale, (pz + 1)*scale);
				vert.m_Tex = GetTexloc( 1);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale + 0.1, (py + 0)*scale, (pz + 0)*scale);
				vert.m_Tex = GetTexloc( 2);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 1)*scale + 0.1, (py + 1)*scale, (pz + 0)*scale);
				vert.m_Tex = GetTexloc( 3);
				m_vlist.Push(vert);
				m_ilist.Push((WORD)itmp + 3);
				m_ilist.Push((WORD)itmp + 1);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 2);
				m_ilist.Push((WORD)itmp + 3);
			}

			//continue;

			if ((dir & (Nyan::Back)) != 0)
			{
				itmp = m_vlist.GetSize();
				//vert.m_Color = 0xffffffff;
				//vert.m_Color_dx11_opengl = 0xffffffff;
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale - 0.1, (py + 1)*scale, (pz + 1)*scale);
				vert.m_Tex = GetTexloc( 0);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale - 0.1, (py + 0)*scale, (pz + 1)*scale);
				vert.m_Tex = GetTexloc( 1);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale - 0.1, (py + 1)*scale, (pz + 0)*scale);
				vert.m_Tex = GetTexloc( 2);
				m_vlist.Push(vert);
				vert.m_Pos = DirectX::XMFLOAT3((x + 0)*scale - 0.1, (py + 0)*scale, (pz + 0)*scale);
				vert.m_Tex = GetTexloc( 3);
				m_vlist.Push(vert);
				m_ilist.Push((WORD)itmp + 3);
				m_ilist.Push((WORD)itmp + 1);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 0);
				m_ilist.Push((WORD)itmp + 2);
				m_ilist.Push((WORD)itmp + 3);
			}
		}
		m_vb->Map();
		m_vb->Fill_Data(m_vlist.GetRaw(), sizeof(VertexType)*m_vsize, 0);
		m_vb->Unmap();

		m_ib->Map();
		m_ib->Fill_Data(m_ilist.GetRaw(), sizeof(WORD)*m_isize, 0);
		m_ib->Unmap();
	}

	void SelectRect::Render(NNN::Shader::c_Effect *effect)
	{
		if (loc == -1)return;
		//class NNN::Shader::c_Effect *effect = NNN::Shader::ShaderLibs::Texture::ColorTexture::GetEffect();

		NNN::Device::DeviceContext::IASetIndexBuffer(m_ib);
		NNN::Device::DeviceContext::IASetVertexBuffers(m_vb, sizeof(VertexType));
		NNN::Device::DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		NNN::Device::DeviceContext::SetEffect(effect, NNN_SHADER_LIBS_TEXTURE_COLORTEXTURE_DX9_TECH_NAME);

		//NNN::Device::DeviceContext::Draw(4);


		NNN::Device::DeviceContext::DrawIndexed(m_isize);

		NNN::Device::DeviceContext::EndEffect();
	}

	SelectRect::~SelectRect()
	{
		SAFE_RELEASE(m_vb);
		SAFE_RELEASE(m_ib);
	}
}