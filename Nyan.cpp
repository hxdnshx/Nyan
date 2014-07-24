//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//
// Desc : 最简单的空白项目
//--------------------------------------------------------------------------------------

#include "MinimalAllocator.hpp"

#define MINIMAL_USE_PROCESSHEAPSTRING
#include "MinimalPath.hpp"

#define MINIMAL_USE_PROCESSHEAPARRAY
#include "MinimalArray.hpp"

#include "Nyan.h"
#include <TCHAR.h>

#include "Scene.h"
#include"Map.h"
#include "Line.h"


using namespace Minimal;

struct NNN::Buffer::s_VertexBuffer	*g_vb = nullptr;
struct NNN::Buffer::s_IndexBuffer *g_ib = nullptr;
class NNN::Texture::c_Texture		*g_texture = nullptr;

class NNN::State::c_RenderState		*g_render_state = nullptr;
class NNN::State::c_SamplerState	*g_sampler_state = nullptr;


DirectX::XMVECTOR g_CamPos = { 10.0f, 10.0f, 10.0f, 0.0f };
DirectX::XMVECTOR g_CamFocus = { 0, 0, 0, 0 };
DirectX::XMVECTOR g_CamAhead = { 0, 0, 1, 0 };
DirectX::XMMATRIX	g_World;
DirectX::XMMATRIX	g_View;
DirectX::XMMATRIX	g_Projection;

WCHAR g_filename[MAX_PATH] = { 0 };
UINT g_image_width = 0;
UINT g_image_height = 0;

struct NNN::Shader::ShaderLibs::Texture::ColorTexture::s_Vertex g_vertices[4];

//Minimal::MinimalArrayT<int> obj(&g_allocator);

WORD g_indices[] =
{
	3,1,0,
	0,2,3,
};

// 窗口标题
const WCHAR g_k_TITLE[] = L"Nyan Map Editor";
//Nyan::Map3D *map;
Nyan::Scene *inst;

void Init_RenderState()
{
	g_render_state = new class NNN::State::c_RenderState();

	g_render_state->RS_SetFillMode(D3D11_FILL_SOLID);
	g_render_state->RS_SetMultisampleEnable(TRUE);
	g_render_state->RS_SetScissorEnable(FALSE);

	g_render_state->DS_SetDepthEnable(TRUE);
	g_render_state->DS_SetDepthFunc(D3D11_COMPARISON_LESS_EQUAL);

	g_render_state->BS_Set_RenderTarget_BlendEnable(TRUE, 0);
	g_render_state->BS_Set_RenderTarget_SrcBlend(D3D11_BLEND_SRC_ALPHA, 0);
	g_render_state->BS_Set_RenderTarget_DestBlend(D3D11_BLEND_INV_SRC_ALPHA, 0);
	g_render_state->BS_Set_RenderTarget_BlendOp(D3D11_BLEND_OP_ADD, 0);
	g_render_state->BS_Set_RenderTarget_SrcBlendAlpha(D3D11_BLEND_ONE, 0);
	g_render_state->BS_Set_RenderTarget_DestBlendAlpha(D3D11_BLEND_ZERO, 0);
	g_render_state->BS_Set_RenderTarget_BlendOpAlpha(D3D11_BLEND_OP_ADD, 0);

	g_render_state->RS_SetCullMode(D3D11_CULL_NONE);
	//g_render_state->RS_SetFrontCounterClockwise(FALSE);
}
//--------------------------------------------------
void Init_SamplerState()
{
	g_sampler_state = new class NNN::State::c_SamplerState();

	g_sampler_state->SetFilter(D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT);
	g_sampler_state->SetAddressU(D3D11_TEXTURE_ADDRESS_CLAMP);
	g_sampler_state->SetAddressV(D3D11_TEXTURE_ADDRESS_CLAMP);
}

/*==============================================================
 * 通用渲染函数
 * Render()
 *==============================================================*/
HRESULT Render(double fTime, float /*fElapsedTime*/, void* /*pUserContext*/)
{
	// Clear render target and the depth stencil
	float ClearColor[4] = { 0, 0, 0, 1.0f }; //{ 0.176f, 0.196f, 0.667f, 1.0f };

	
	

	NNN::Device::DeviceContext::ClearRenderTargetView(ClearColor);
	NNN::Device::DeviceContext::ClearDepthStencilView(D3D11_CLEAR_DEPTH, 1.0f, 0);
	NNN::Device::DeviceContext::IASetIndexBuffer(g_ib);
	NNN::Device::DeviceContext::IASetVertexBuffers(g_vb, sizeof(struct NNN::Shader::ShaderLibs::Texture::ColorTexture::s_Vertex));
	NNN::Device::DeviceContext::IASetInputLayout(NNN::Shader::ShaderLibs::Texture::ColorTexture::GetInputLayout());
	NNN::Device::DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	class NNN::Shader::c_Effect *effect = NNN::Shader::ShaderLibs::Texture::ColorTexture::GetEffect();


	DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	g_View = DirectX::XMMatrixLookAtLH(g_CamPos, g_CamFocus, g_CamAhead);
	DirectX::XMMATRIX mWVP = g_World * g_View * g_Projection;
	effect->SetMatrix("g_mWVP", (const float*)&mWVP);

	effect->SetResource("g_Texture", g_texture, 0);

	// 设置渲染状态
	NNN::State::SetRenderState(g_render_state);

	// 设置采样状态
	NNN::State::SetSamplerState(g_sampler_state, 0, 0, g_texture);

	NNN::Device::DeviceContext::SetEffect(effect, NNN_SHADER_LIBS_TEXTURE_COLORTEXTURE_DX9_TECH_NAME);
	NNN::Device::DeviceContext::DrawIndexed(6, 0, 0, 4);
	//NNN::Device::DeviceContext::Draw(4);
	NNN::Device::DeviceContext::EndEffect();

	inst->Render(0);

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
	static WCHAR s_title_txt[100] = {0};

	WCHAR title_txt[100];
	swprintf_s(title_txt, L"%s [ %s ]", g_k_TITLE, NNN::DXUTGetFrameStats(true));
	if(wcscmp(s_title_txt, title_txt))
	{
		swprintf_s(s_title_txt, L"%s", title_txt);
		SetWindowText(NNN::DXUTGetHWND(), title_txt);
	}
#endif	// NNN_PLATFORM_WIN32

	return S_OK;
}




void ResetMap()
{
	inst->GetMap()->At(0, 0, 0).TexType = 0;
	inst->GetMap()->At(0, 0, 1).TexType = 0;
	inst->GetMap()->At(0, 0, 2).TexType = 0;
	inst->GetMap()->At(0, 0, 3).TexType = 0;
	inst->GetMap()->At(0, 1, 0).TexType = 0;
	inst->GetMap()->At(0, 2, 0).TexType = 0;
	inst->GetMap()->At(0, 3, 0).TexType = 0;
	inst->GetMap()->At(1, 0, 0).TexType = 0;
	inst->GetMap()->At(2, 0, 0).TexType = 0;
	inst->GetMap()->At(3, 0, 0).TexType = 0;

	inst->GetMap()->At(3, 1, 0).TexType = 0;
	inst->GetMap()->At(3, 2, 0).TexType = 0;
	inst->GetMap()->At(3, 3, 0).TexType = 0;
	inst->GetMap()->At(3, 0, 1).TexType = 0;
	inst->GetMap()->At(3, 0, 2).TexType = 0;
	inst->GetMap()->At(3, 0, 3).TexType = 0;
	inst->GetMap()->At(3, 1, 3).TexType = 0;
	inst->GetMap()->At(3, 2, 3).TexType = 0;
	inst->GetMap()->At(3, 3, 3).TexType = 0;

	inst->GetMap()->At(1, 3, 0).TexType = 0;
	inst->GetMap()->At(2, 3, 0).TexType = 0;
	inst->GetMap()->At(3, 3, 0).TexType = 0;
	inst->GetMap()->At(0, 3, 1).TexType = 0;
	inst->GetMap()->At(0, 3, 2).TexType = 0;
	inst->GetMap()->At(0, 3, 3).TexType = 0;
	inst->GetMap()->At(1, 3, 3).TexType = 0;
	inst->GetMap()->At(2, 3, 3).TexType = 0;
	inst->GetMap()->At(3, 3, 3).TexType = 0;

	inst->GetMap()->At(1, 0, 3).TexType = 0;
	inst->GetMap()->At(2, 0, 3).TexType = 0;
	inst->GetMap()->At(3, 0, 3).TexType = 0;
	inst->GetMap()->At(0, 1, 3).TexType = 0;
	inst->GetMap()->At(0, 2, 3).TexType = 0;
	inst->GetMap()->At(0, 3, 3).TexType = 0;
	inst->GetMap()->At(3, 3, 1).TexType = 0;
	inst->GetMap()->At(3, 3, 2).TexType = 0;
	inst->GetMap()->At(3, 3, 3).TexType = 2;
}

/*==============================================================
 * 初始化函数
 * OnCreate_func()
 *==============================================================*/
void OnCreate_func(void* /*pUserContext*/)
{
	HRESULT  hr;
	// TODO: 初始化代码

	g_World = DirectX::XMMatrixIdentity();

	inst = new Nyan::Scene(&g_allocator);

	inst->InitScene(256,256,50);
	
	ResetMap();

	inst->ImportTexture(L"Texture_Default.png");
	inst->ImportTexture(L"Texture_Selected.png");
	inst->ImportTexture(L"Texture_Special.png");

	inst->GetMap()->ReCalcMask();
 	inst->InitBuffer(1);
	
	//g_View = NNN::Misc::GetOrthoView();
	
	//V(NNN::Texture::Add(L"Character.png", L"Character.png", 0xffff00ff, true));
	V(NNN::Texture::Add(L"Texture_Default.png", L"Texture_Default.png", 0xffff00ff, true));
	g_texture = NNN::Texture::Find(L"Texture_Default.png");

	g_image_width = 10;
	g_image_height = 10;

	float min_width = -(float)g_image_width / 2;
	float max_width = min_width + g_image_width;
	float min_height = -(float)g_image_height / 2;
	float max_height = min_height + g_image_height;

	float max_u = g_texture->GetMaxU();
	float max_v = g_texture->GetMaxV();

	g_vertices[0].m_Pos = DirectX::XMFLOAT3(min_width, max_height, 0.0f);
	g_vertices[0].m_Tex = DirectX::XMFLOAT2(0.0f, 0.0f);
	g_vertices[0].m_Color = 0xffffffff;

	g_vertices[1].m_Pos = DirectX::XMFLOAT3(max_width, max_height, 0.0f);
	g_vertices[1].m_Tex = DirectX::XMFLOAT2(max_u, 0.0f);
	g_vertices[1].m_Color = 0xffffffff;

	g_vertices[2].m_Pos = DirectX::XMFLOAT3(min_width, min_height, 0.0f);
	g_vertices[2].m_Tex = DirectX::XMFLOAT2(0.0f, max_v);
	g_vertices[2].m_Color = 0xffffffff;

	g_vertices[3].m_Pos = DirectX::XMFLOAT3(max_width, min_height, 0.0f);
	g_vertices[3].m_Tex = DirectX::XMFLOAT2(max_u, max_v);
	g_vertices[3].m_Color = 0xffffffff;

	NNN::es_GraphAPI graph_api = NNN::GetGraphAPI();

	for (UINT i = 0; i < _countof(g_vertices); ++i)
	{
		g_vertices[i].UpdateData();

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
		if (graph_api == NNN::es_GraphAPI::DX9)
		{
			g_vertices[i].m_Pos.x -= 0.5f;
			g_vertices[i].m_Pos.y -= 0.5f;
		}
#endif	// NNN_PLATFORM_WIN32
	}

	
	g_vb = new struct NNN::Buffer::s_VertexBuffer();
	g_ib = new struct NNN::Buffer::s_IndexBuffer();

	switch (graph_api)
	{
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
	case NNN::es_GraphAPI::DX11:
		V(g_vb->Init_DX11(g_vertices, sizeof(g_vertices)));
		V(g_ib->Init_DX11(g_indices, sizeof(g_indices)));
		break;
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
	case NNN::es_GraphAPI::DX9:
		V(g_vb->Init_DX9(g_vertices, sizeof(g_vertices)));
		V(g_ib->Init_DX9(g_indices, sizeof(g_indices)));
		break;
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
	case NNN::es_GraphAPI::OpenGL:
		V(g_vb->Init_OpenGL(g_vertices, sizeof(g_vertices), GL_STATIC_DRAW));
		V(g_ib->Init_OpenGL(g_indices, sizeof(g_indices), GL_STATIC_DRAW));
		break;
#endif	// !NNN_PLATFORM_WP8
	}

	ChangeSize(NNN::Misc::GetClientWidth(), NNN::Misc::GetClientHeight());

	Init_RenderState();
	Init_SamplerState();

	// 重置计时器
	NNN::Misc::ResetTimer();
}


/*==============================================================
 * 清理函数
 * OnDestroy_func()
 *==============================================================*/
void OnDestroy_func(void* /*pUserContext*/)
{
	delete inst;

	SAFE_RELEASE(g_vb);
	SAFE_RELEASE(g_ib);
	
	NNN::Texture::Release(g_texture);
	g_texture = nullptr;

	SAFE_DELETE(g_render_state);
	SAFE_DELETE(g_sampler_state);

}


#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
/*==============================================================
 * OnReset_dx9()	- Reset
 * OnLost_dx9()		- Lost
 *==============================================================*/
HRESULT OnReset_dx9(IDirect3DDevice9* /*pd3dDevice*/, const D3DSURFACE_DESC* /*pBackBufferSurfaceDesc*/, void* /*pUserContext*/)
{
	return S_OK;
}
//--------------------------------------------------
void OnLost_dx9(void* /*pUserContext*/)
{
}
#endif	// NNN_PLATFORM_WIN32


/*==============================================================
 * 改变分辨率大小
 * ChangeSize()
 *==============================================================*/
void ChangeSize(UINT width, UINT height)
{
	if (width > 0 && height > 0)
		g_Projection = NNN::Misc::GetPerspectiveFovLH((int)width, (int)height);
}


/*==============================================================
 * 处理按键
 * OnKeyboard()
 *
 *		nChar		- 按键（例如：'A'、'a'、VK_LEFT、VK_RIGHT）
 *		bKeyDown	- 是否 WM_KEYDOWN 或 WM_SYSKEYDOWN
 *		bAltDown	- Alt 是否按下
 *==============================================================*/
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool /*bAltDown*/, void* /*pUserContext*/ )
{
	static bool onceflag_I=false, onceflag_R=false;
	if (!bKeyDown)
	{
		return;
	}
	if (nChar=='I' || nChar=='i')//(NNN::Input::Keyboard::isKeyDown(DIK_I))
	{
		if (!onceflag_I)
		{
			//onceflag_I = true;
			{
				DirectX::XMVECTOR m_mouse = DirectX::XMVectorSet(0, 0, 0, 1);
				m_mouse = DirectX::XMVector4Transform(m_mouse, DirectX::XMMatrixInverse(nullptr, g_View));

				DirectX::XMVECTOR m_mouse1 = DirectX::XMVectorSet((
					(float)NNN::Input::Mouse::MouseX() *2 / NNN::Misc::GetClientWidth(false) - 1) /
					g_Projection.r[0].m128_f32[0], 
					-((float)NNN::Input::Mouse::MouseY() * 2 / NNN::Misc::GetClientHeight(false) - 1) /
					g_Projection.r[1].m128_f32[1], 1, 0);
				DirectX::XMMATRIX viewinv = DirectX::XMMatrixInverse(nullptr, g_View);
				m_mouse1 = DirectX::XMVector4Transform(m_mouse1, viewinv);

				DirectX::XMFLOAT4 ori, dir;
				DirectX::XMStoreFloat4(&ori, m_mouse);
				DirectX::XMStoreFloat4(&dir, DirectX::XMVector4Normalize(m_mouse1));
				DirectX::XMFLOAT4 result = inst->TestCollisoin(Nyan::LineFunc(ori, dir));
				if (result.x != -1)
				{
					int nx, ny, nz;
					nx = result.x;
					ny = result.y;
					nz = result.z;
					switch ((int)result.w)
					{
					case Nyan::Direction::Up:
						nz++;
						break;
					case Nyan::Direction::Down:
						nz--;
						break;
					case Nyan::Direction::Left:
						ny--;
						break;
					case Nyan::Direction::Right:
						ny++;
						break;
					case Nyan::Direction::Front:
						nx++;
						break;
					case Nyan::Direction::Back:
						nx--;
						break;
					}
					if (nx < 0 || nx >= inst->GetMap()->GetX())return;
					if (ny < 0 || ny >= inst->GetMap()->GetY())return;
					if (nz < 0 || nz >= inst->GetMap()->GetY())return;
					if (inst->GetMap()->At(nx, ny, nz).TexType == -1)
					{
						inst->GetMap()->ReCalcBlock(nx, ny, nz, inst->GetMap()->At(nx, ny, nz).TexType, 0);
						inst->GetMap()->At(nx, ny, nz).TexType = 0;
						inst->InitBuffer(1.0);
					}
				}
				//{result.x},{result.y},{result.z},{result.w}\n
			}
		}
	}
	else
	{
		onceflag_I = false;
	}
	
	if (nChar=='R' || nChar=='r')//(NNN::Input::Keyboard::isKeyDown(DIK_R))
	{
		if (!onceflag_R)
		{
			//onceflag_R = true;
			{
				DirectX::XMVECTOR m_mouse = DirectX::XMVectorSet(0, 0, 0, 1);
				m_mouse = DirectX::XMVector4Transform(m_mouse, DirectX::XMMatrixInverse(nullptr, g_View));

				DirectX::XMVECTOR m_mouse1 = DirectX::XMVectorSet((
					(float)NNN::Input::Mouse::MouseX() * 2 / NNN::Misc::GetClientWidth(false) - 1) /
					g_Projection.r[0].m128_f32[0],
					-((float)NNN::Input::Mouse::MouseY() * 2 / NNN::Misc::GetClientHeight(false) - 1) /
					g_Projection.r[1].m128_f32[1], 1, 0);
				DirectX::XMMATRIX viewinv = DirectX::XMMatrixInverse(nullptr, g_View);
				m_mouse1 = DirectX::XMVector4Transform(m_mouse1, viewinv);

				DirectX::XMFLOAT4 ori, dir;
				DirectX::XMStoreFloat4(&ori, m_mouse);
				DirectX::XMStoreFloat4(&dir, DirectX::XMVector4Normalize(m_mouse1));
				DirectX::XMFLOAT4 result = inst->TestCollisoin(Nyan::LineFunc(ori, dir));
				if (result.x != -1)
				{
					
					if (inst->GetMap()->At(result.x, result.y, result.z).TexType != -1)
					{
						inst->GetMap()->ReCalcBlock(result.x, result.y, result.z, inst->GetMap()->At(result.x, result.y, result.z).TexType, -1);
						inst->GetMap()->At(result.x, result.y, result.z).TexType = -1;
						inst->GetMap()->ClearMask();
						inst->InitBuffer(1.0);
					}
				}
				//{result.x},{result.y},{result.z},{result.w}\n
			}
		}
	}
	else
	{
		onceflag_R = false;
	}
}


/*==============================================================
 * 处理鼠标/单触点
 * OnMouse()
 *==============================================================*/
void CALLBACK OnMouse(	bool /*bLeftButtonDown*/, bool /*bRightButtonDown*/, bool /*bMiddleButtonDown*/,
						bool /*bSideButton1Down*/, bool /*bSideButton2Down*/, int /*nMouseWheelDelta*/,
						int /*xPos*/, int /*yPos*/, void* /*pUserContext*/ )
{
}


/*==============================================================
 * 处理多触点
 * OnMultiTouch()
 *==============================================================*/
void CALLBACK OnMultiTouch(	struct NNN::Input::MultiTouch::s_TouchPoint* /*touches*/, int /*touches_count*/,
							NNN::Input::es_InputMessageType /*msg*/, POINT /*point*/, UINT /*touch_id*/,
							void* /*pUserContext*/ )
{
}


/*==============================================================
 * 处理旋转屏幕
 * OnRotation()
 *==============================================================*/
void CALLBACK OnRotation(	NNN::Input::Rotation::es_Rotation /*rotation*/,
							NNN::Input::Rotation::es_Rotation /*last_rotation*/,
							void* /*pUserContext*/ )
{
}


/*==============================================================
 * 处理加速度计
 * OnAccelerometer()
 *==============================================================*/
void CALLBACK OnAccelerometer( double /*x*/, double /*y*/, double /*z*/, void* /*pUserContext*/ )
{
}


/*==============================================================
 * 处理方向感应器
 * OnOrientation()
 *==============================================================*/
void CALLBACK OnOrientation( float /*x*/, float /*y*/, float /*z*/, void* /*pUserContext*/ )
{
}


/*==============================================================
 * 处理陀螺测试仪
 * OnGyrometer()
 *==============================================================*/
void CALLBACK OnGyrometer(	double /*AngularVelocityX*/, double /*AngularVelocityY*/, double /*AngularVelocityZ*/,
							void* /*pUserContext*/ )
{
}


/*==============================================================
 * 处理测斜仪
 * OnInclinometer()
 *==============================================================*/
void CALLBACK OnInclinometer( float /*PitchDegrees*/, float /*RollDegrees*/, float /*YawDegrees*/, void* /*pUserContext*/ )
{
}


/*==============================================================
 * 处理地磁仪
 * OnMagnetometer()
 *==============================================================*/
void CALLBACK OnMagnetometer( float /*x*/, float /*y*/, float /*z*/, void* /*pUserContext*/ )
{
}


/*==============================================================
 * 处理指南针
 * OnCompass()
 *==============================================================*/
void CALLBACK OnCompass( double /*MagneticNorth*/, double /*TrueNorth*/, void* /*pUserContext*/ )
{
}


#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
/*==============================================================
 * 处理应用程序的消息处理
 * MsgProc()
 *==============================================================*/
LRESULT CALLBACK MsgProc(	HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
							bool* /*pbNoFurtherProcessing*/, void* /*pUserContext*/ )
{
	return 0;
}
#endif	// NNN_PLATFORM_WIN32


#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
/*==============================================================
 * 在 D3D9 或 D3D11 设备创建之前，修改设备的设置
 * ModifyDeviceSettings()
 *==============================================================*/
bool CALLBACK ModifyDeviceSettings( NNN::DXUTDeviceSettings* pDeviceSettings, void* /*pUserContext*/ )
{
	UNREFERENCED_PARAMETER(pDeviceSettings);

	// Uncomment this to get debug information from D3D11
	//pDeviceSettings->d3d11.CreateFlags |= D3D11_CREATE_DEVICE_DEBUG;

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if( s_bFirstTime )
	{
		s_bFirstTime = false;
		if( ( NNN::DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
			pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE ) )
		{
			NNN::Optional::DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
		}
	}
#endif	// NNN_PLATFORM_WIN32

	return true;
}


/*==============================================================
 * 当设备被移除时调用。返回 true 时，表示找到一个新设备；false 表示退出
 * OnDeviceRemoved()
 *==============================================================*/
bool CALLBACK OnDeviceRemoved( void* /*pUserContext*/ )
{
	return true;
}
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8


/*==============================================================
 * 处理场景更新。无论使用哪种图形 API，也会调用这个函数。
 * OnFrameMove()
 *==============================================================*/
void CALLBACK OnFrameMove( double /*fTime*/, float /*fElapsedTime*/, void* /*pUserContext*/ )
{
	static int m_lx = -1, m_ly = -1;
	int dx = 0, dy = 0, dz = 0;


	dz = NNN::Input::Mouse::MouseZ_Delta();
	if (dz != 0)
	{
		float scale = 1.0 + (float)dz / 1200;
		DirectX::XMVECTOR m_pos = DirectX::XMVectorSubtract(g_CamPos, g_CamFocus);
		m_pos = DirectX::XMVectorScale(m_pos, scale);
		g_CamPos = DirectX::XMVectorAdd(g_CamFocus, m_pos);
	}


	dx = NNN::Input::Mouse::MouseX() - m_lx;
	dy = NNN::Input::Mouse::MouseY() - m_ly;
	if (m_lx == -1)
	{
		m_lx = NNN::Input::Mouse::MouseX();
		m_ly = NNN::Input::Mouse::MouseY();
		return;
	}
	if (dx == 0 && dy == 0)
	{
		//return;
	}
	m_lx = NNN::Input::Mouse::MouseX();
	m_ly = NNN::Input::Mouse::MouseY();

	{
		static DirectX::XMFLOAT4 l_r = { -1, -1, -1, -1 };
		DirectX::XMVECTOR m_mouse = DirectX::XMVectorSet(0, 0, 0, 1);
		m_mouse = DirectX::XMVector4Transform(m_mouse, DirectX::XMMatrixInverse(nullptr, g_View));

		DirectX::XMVECTOR m_mouse1 = DirectX::XMVectorSet(((float)m_lx * 2 / NNN::Misc::GetClientWidth(false) - 1) / g_Projection.r[0].m128_f32[0], -((float)m_ly * 2 / NNN::Misc::GetClientHeight(false) - 1) / g_Projection.r[1].m128_f32[1], 1, 0);
		DirectX::XMMATRIX viewinv = DirectX::XMMatrixInverse(nullptr, g_View);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, viewinv);

		DirectX::XMFLOAT4 ori, dir;
		DirectX::XMStoreFloat4(&ori, m_mouse);
		DirectX::XMStoreFloat4(&dir, DirectX::XMVector4Normalize(m_mouse1));
		DirectX::XMFLOAT4 result = inst->TestCollisoin(Nyan::LineFunc(ori, dir));
		if (l_r.x != -1)
		{
			if (inst->GetMap()->At(l_r.x, l_r.y, l_r.z).TexType != -1)
			{
				inst->GetMap()->ReCalcBlock(l_r.x, l_r.y, l_r.z, inst->GetMap()->At(l_r.x, l_r.y, l_r.z).TexType, 0);
				inst->GetMap()->At(l_r.x, l_r.y, l_r.z).TexType = 0;
			}
			l_r.x = -1;
			l_r.y = -1;
			l_r.z = -1;
			//inst->InitBuffer(1.0);
		}
		if (result.x != -1)
		{
			inst->GetMap()->ReCalcBlock(result.x, result.y, result.z, inst->GetMap()->At(result.x, result.y, result.z).TexType, 2);
			inst->GetMap()->At(result.x, result.y, result.z).TexType = 2;
			l_r = result;
			
		}
		inst->InitBuffer(1.0);
		//{result.x},{result.y},{result.z},{result.w}\n
	}

	//static float p_x, p_y;
	if (NNN::Input::Mouse::isMouseButtonDown(1)){
		
		DirectX::XMVECTOR m_pos = DirectX::XMVectorSubtract(g_CamPos,g_CamFocus);
		float length=DirectX::XMVectorGetX(DirectX::XMVector3Length(m_pos));
		DirectX::XMVECTOR m_n1 = DirectX::XMVectorSet(0, 0, 1, 0);
		DirectX::XMVECTOR m_n2 = DirectX::XMVectorSet(0, 1, 0, 0);
		m_n1 = DirectX::XMVectorAbs(DirectX::XMVector3Cross(m_n1, m_pos));
		if (DirectX::XMVectorGetX(DirectX::XMVector3Length(m_n1)) < 1.00)
		{
			//return;
		}
		m_pos = DirectX::XMVector4Transform(m_pos, DirectX::XMMatrixRotationAxis(m_n1,(float)dy / 150 * 3.14159));
		m_n2 = DirectX::XMVector3Cross(m_n2, m_pos);
		m_pos = DirectX::XMVector4Transform(m_pos, DirectX::XMMatrixRotationAxis(m_n2,(float)dx / 150 * 3.14159));
		m_pos = DirectX::XMVector4Normalize(m_pos);
		m_pos = DirectX::XMVectorScale(m_pos, length);
		g_CamPos= DirectX::XMVectorAdd(m_pos, g_CamFocus);
		

		
	}
	else if (NNN::Input::Mouse::isMouseButtonDown(0))
	{

		{
			DirectX::XMVECTOR m_pos = DirectX::XMVectorSubtract(g_CamPos, g_CamFocus);
			//DirectX::XMVECTOR mousedelta = DirectX::XMVectorSet((float)- dy / 100, (float)/*dx*/ 0 / 100, 0, 0); //旋转90度
			DirectX::XMVECTOR n = DirectX::XMVector4Normalize(m_pos);//法向量
			DirectX::XMVECTOR mousedelta = DirectX::XMVectorScale(n, (float)dy / 50);
			g_CamPos = DirectX::XMVectorAdd(g_CamPos, mousedelta);
			g_CamFocus = DirectX::XMVectorAdd(g_CamFocus, mousedelta);
		}

		{
			DirectX::XMVECTOR m_pos = DirectX::XMVectorSubtract(g_CamFocus, g_CamPos);
			float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(m_pos));
			DirectX::XMVECTOR mousedelta = DirectX::XMVectorSet((float)-/*dy*/ 0 / 50 , (float)/*dx*/ dy / 50 , 0, 0);//旋转90度
			DirectX::XMVECTOR n = DirectX::XMVector4Normalize(m_pos);//法向量
			mousedelta = DirectX::XMVector3Cross(mousedelta, n);
			m_pos = DirectX::XMVectorAdd(m_pos, mousedelta);
			m_pos = DirectX::XMVector4Normalize(m_pos);
			m_pos = DirectX::XMVectorScale(m_pos, length);
			g_CamFocus = DirectX::XMVectorAdd(m_pos, g_CamPos);
		}
	}
	
}


/*==============================================================
 * 入口点调用的函数
 * main_func()
 *==============================================================*/
void CALLBACK main_func()
{
	NNN::Init::Step1::DoInit();

#if (NNN_PLATFORM == NNN_PLATFORM_ANDROID)
#if defined(DEBUG) || defined(_DEBUG)
	NNN::Platform::Android::Add_ShowDebugInfoFlag(NNN::Platform::Android::es_DebugInfo::Pointer);
#endif	// DEBUG || _DEBUG
#endif	// NNN_PLATFORM_ANDROID

	// 加载数据包
#if (NNN_PLATFORM == NNN_PLATFORM_ANDROID)
	WCHAR sdcard_path[MAX_PATH] = {0};
	NNN::Text::char2wchar(NNN::Platform::Android::get_sdcard_path(), sdcard_path, _countof(sdcard_path));
#endif	// NNN_PLATFORM_ANDROID

	WCHAR *dir_list[] =
	{
		L"",
		L"Files/",
		L"../Files/",
		L"../../Files/",
		L"../../../Files/",
		L"../../../../Files/",
#if (NNN_PLATFORM == NNN_PLATFORM_ANDROID)
		L"/",
		sdcard_path,
#elif (NNN_PLATFORM == NNN_PLATFORM_IOS)
		L"../Documents/",
#elif (NNN_PLATFORM == NNN_PLATFORM_WP8)
		L"/",
#endif	// NNN_PLATFORM
	};

	WCHAR *file_list[] =
	{
		L"nnnEngine.whp",
		L"Nyan.whp",
	};

	std::vector<std::wstring> packet_list;
	for(int dir_idx=0; dir_idx<_countof(dir_list); ++dir_idx)
	{
		for(int file_idx=0; file_idx<_countof(file_list); ++file_idx)
		{
			std::wstring file = dir_list[dir_idx];
			file += file_list[file_idx];

			packet_list.push_back(file);
		}
	}
	NNN::Init::Step2::LoadCustomPacket(&packet_list);

	// 设置回调函数
	NNN::Init::Step2::FrameMove(OnFrameMove);

	NNN::Init::Step2::Keyboard(OnKeyboard);
	NNN::Init::Step2::Mouse(OnMouse);
	NNN::Init::Step2::MultiTouch(OnMultiTouch);
	NNN::Init::Step2::Rotation(OnRotation);
	NNN::Init::Step2::Accelerometer(OnAccelerometer);
	NNN::Init::Step2::Orientation(OnOrientation);
	NNN::Init::Step2::Gyrometer(OnGyrometer);
	NNN::Init::Step2::Inclinometer(OnInclinometer);
	NNN::Init::Step2::Magnetometer(OnMagnetometer);
	NNN::Init::Step2::Compass(OnCompass);

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
	NNN::Init::Step2::MsgProc(MsgProc);
#endif	// NNN_PLATFORM_WIN32
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
	NNN::Init::Step2::DeviceChanging(ModifyDeviceSettings);
	NNN::Init::Step2::DeviceRemoved(OnDeviceRemoved);

	// DX11 回调
	NNN::Init::Step2::D3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
	NNN::Init::Step2::D3D11DeviceCreated(OnD3D11CreateDevice);
	NNN::Init::Step2::D3D11SwapChainResized(OnD3D11ResizedSwapChain);
	NNN::Init::Step2::D3D11FrameRender(OnD3D11FrameRender);
	NNN::Init::Step2::D3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
	NNN::Init::Step2::D3D11DeviceDestroyed(OnD3D11DestroyDevice);
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
	// DX9 回调
	NNN::Init::Step2::D3D9DeviceAcceptable(IsD3D9DeviceAcceptable);
	NNN::Init::Step2::D3D9DeviceCreated(OnD3D9CreateDevice);
	NNN::Init::Step2::D3D9DeviceReset(OnD3D9ResetDevice);
	NNN::Init::Step2::D3D9FrameRender(OnD3D9FrameRender);
	NNN::Init::Step2::D3D9DeviceLost(OnD3D9LostDevice);
	NNN::Init::Step2::D3D9DeviceDestroyed(OnD3D9DestroyDevice);
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
	// OpenGL 回调
	NNN::Init::Step2::OpenglInit(OpenGL_Init);
	NNN::Init::Step2::OpenglFinal(OpenGL_Final);
	NNN::Init::Step2::OpenglChangeSize(OpenGL_ChangeSize);
	NNN::Init::Step2::OpenglFrameRender(OpenGL_OnFrameRender);
#endif	// !NNN_PLATFORM_WP8

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
	NNN::Init::Step2::SetCursorSettings(true, true);
	NNN::Init::Step2::SetHotkeyHandling();
#endif	// NNN_PLATFORM_WIN32
	NNN::Init::Step2::Create_Window();

	NNN::Init::Step3::CreateDevice();
	NNN::Init::Step4::MainLoop();	// Enter into the DXUT render loop
}


/*==============================================================
 * Win32 入口点
 * WinMain()
 *==============================================================*/
#if (NNN_PLATFORM != NNN_PLATFORM_WIN32)
NNN_INIT_FUNC(main_func);
#else
int WINAPI WinMain( HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/ )
{
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
	// 设置 dump 文件（程序崩溃后记录一些调试信息）
	NNN::Misc::SetDumpFile(L"Nyan.dmp");
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
	// 内存泄漏检测
	NNN::Misc::MemoryLeakCheck();
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

	main_func();

	return NNN::DXUTGetExitCode();
}
#endif	// !NNN_PLATFORM_WIN32
