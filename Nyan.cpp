﻿//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//
// Desc : 最简单的空白项目
//--------------------------------------------------------------------------------------

#include <boost\random.hpp>

#include "MinimalAllocator.hpp"

#include "dxgi.h"

#define MINIMAL_USE_PROCESSHEAPSTRING
#include "MinimalPath.hpp"

#define MINIMAL_USE_PROCESSHEAPARRAY
#include "MinimalArray.hpp"

#include "Nyan.h"
#include <TCHAR.h>
#include "Scene2RayAdapter.h"
#include "Scene.h"
#include"Map.h"
#include "Line.h"
#include "SelectRect.h"
#include "TimerSys.h"
#include ".\..\..\3rdParty\LibPNG\Src\png.h"

#define ImgFormat png_byte


# define FastHack


using namespace std::placeholders;
using namespace Minimal;




class NNN::State::c_RenderState		*g_render_state = nullptr;
class NNN::State::c_SamplerState	*g_sampler_state = nullptr;
class NNN::Shader::c_Effect *g_effect;
class NNN::GUI::c_GUI_System *g_gui;
class NNN::Shader::s_InputLayout *g_layout;

boost::mt19937 randomx(114514);
boost::uniform_int<int> normdist(0,39);
boost::variate_generator<boost::mt19937&, boost::uniform_int<int> > sampler(randomx,normdist);


//DirectX::XMVECTOR g_CamPos = { 10.0f, 10.0f, 10.0f, 0.0f };
//DirectX::XMVECTOR g_CamFocus = { 0, 0, 0, 0 };
//DirectX::XMVECTOR g_CamAhead = { 0, 0, 1, 0 };
NNN::Camera::c_FirstPersonCamera g_cam;
DirectX::XMMATRIX	g_World;
DirectX::XMMATRIX	g_View;
DirectX::XMMATRIX	g_Projection;
Nyan::SelectRect *rect;

WCHAR g_filename[MAX_PATH] = { 0 };
UINT g_image_width = 0;
UINT g_image_height = 0;
int ctex = 0;
int frame = 0;
float ddx = 0;
float ddy = 0;
float ddz = 0;
#ifdef FastHack
float tdx = -1;
float tdy = -1;
float tdz = -1;
float lightdist = 50;
bool lock = false;
DirectX::XMFLOAT4 LightConeDir;
Nyan::Scene2RayAdapter *Adapter;
#endif
int mode = 0;
Nyan::TimerManage Tim;




//Minimal::MinimalArrayT<int> obj(&g_allocator);

WORD g_indices[] =
{
	3,1,0,
	0,2,3,
};

// 窗口标题
const WCHAR g_k_TITLE[] = L"Nyan";
//Nyan::Map3D *map;
Nyan::Scene *inst;
Nyan::Scene *character;



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


ID3D11Buffer *p_ByteAddrBuffer;
ID3D11ShaderResourceView *p_ByteAddrBufferView;

HRESULT CreateStructureBuffer(ID3D11Device* pDevice, UINT elementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufferOut)
{
	*ppBufferOut = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = elementSize*uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = elementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufferOut);
	}
	else
		return pDevice->CreateBuffer(&desc, NULL, ppBufferOut);
}

//利用ID3D11Device::CreateShaderResouceView()来创建GPU中Buffer的resourceView
HRESULT CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	//假定这是个structure buffer
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.BufferEx.NumElements = descBuf.ByteWidth/16;

	return pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

void Init_Buffer()
{
	int mem[8192/4];
	D3D11_SUBRESOURCE_DATA dat;
	for (int i = 0; i < 8192/4; ++i)
	{
		mem[i] = 2;
	}
	dat.pSysMem = mem;
	dat.SysMemPitch = 0;
	dat.SysMemSlicePitch = 0;
	ID3D11Buffer *pStructuredBuffer;   // Create Structured Buffer 
	D3D11_BUFFER_DESC sbDesc;  
	sbDesc.BindFlags            = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;  
	sbDesc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;
	sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;//D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	sbDesc.StructureByteStride  = 4;  //This Structure is not represents a structured buffer
	sbDesc.ByteWidth            = 8192; // Test 
	sbDesc.Usage                = D3D11_USAGE_DEFAULT;
	//在完成测试后请移动到单独的类中
	auto p_device = NNN::DXUTGetD3D11Device();
	auto r2=p_device->CreateBuffer(&sbDesc, &dat, &p_ByteAddrBuffer);
	//auto r = CreateBufferSRV(NNN::DXUTGetD3D11Device(), p_ByteAddrBuffer, &p_ByteAddrBufferView);
	//return;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	memset(&srDesc, 0, sizeof(srDesc));
	srDesc.Format = DXGI_FORMAT_UNKNOWN;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srDesc.BufferEx.Flags = 0;//D3D11_BUFFEREX_SRV_FLAG_RAW;
	srDesc.BufferEx.NumElements = 8192/16;
	srDesc.BufferEx.FirstElement = 0;
	auto r = p_device->CreateShaderResourceView(p_ByteAddrBuffer,&srDesc,&p_ByteAddrBufferView);// , &srDesc, NULL);
	if (r == NULL)
	{
		int i = 1;
		return;
	}
}

void Init_Bufferx()
{
	HRESULT r = CreateStructureBuffer(NNN::DXUTGetD3D11Device(), 4, 8192, nullptr, &p_ByteAddrBuffer);
	HRESULT r1 = CreateBufferSRV(NNN::DXUTGetD3D11Device(), p_ByteAddrBuffer, &p_ByteAddrBufferView);
}

void Buffer_FillData()
{
	return;//下面的代码是不可达的
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	char* p_data;
	NNN::DXUTGetD3D11DeviceContext()->Map(p_ByteAddrBuffer, 0, D3D11_MAP_WRITE, 0, &mappedRes);
	p_data = (char*)mappedRes.pData;
	
	NNN::DXUTGetD3D11DeviceContext()->Unmap(p_ByteAddrBuffer, 0);
}

/*==============================================================
 * 通用渲染函数
 * Render()
 *==============================================================*/
HRESULT Render(double fTime, float fElapsedTime, void* /*pUserContext*/)
{
	// Clear render target and the depth stencil
	float ClearColor[4] = { 0, 0, 0, 1.0f }; //{ 0.176f, 0.196f, 0.667f, 1.0f };

	
	

	NNN::Device::DeviceContext::ClearRenderTargetView(ClearColor);
	NNN::Device::DeviceContext::ClearDepthStencilView(D3D11_CLEAR_DEPTH, 1.0f, 0);
	//NNN::Device::DeviceContext::IASetIndexBuffer(g_ib);
	//NNN::Device::DeviceContext::IASetVertexBuffers(g_vb, sizeof(struct NNN::Shader::ShaderLibs::Texture::ColorTexture::s_Vertex));
	NNN::Device::DeviceContext::IASetInputLayout(g_layout);
	//NNN::Device::DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	NNN::DXUTGetD3D11DeviceContext()->VSSetShaderResources(0, 1, &p_ByteAddrBufferView);

	class NNN::Shader::c_Effect *effect = g_effect;
	float light_dir[] = { ddx, ddy, ddz+1, lightdist };
	float lightcone_dir[] = { ddx - LightConeDir.x, ddy - LightConeDir.y, ddz +1 - LightConeDir.z, 1.15f };
	effect->SetFloatVector("g_Light", light_dir, 4);
	effect->SetFloatVector("g_LightDir", lightcone_dir, 4);




	//DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	g_cam.FrameMove(fElapsedTime);
	g_View = g_cam.GetViewMatrix();
	g_World = DirectX::XMMatrixTranslation(0, 0, 0);
	DirectX::XMMATRIX mRot = DirectX::XMMatrixRotationX(-0.5*3.141592);
	DirectX::XMMATRIX mWVP =g_World * mRot * g_View * g_Projection;
	//effect->SetMatrix("g_mWVP", (const float*)&mWVP);

	//effect->SetResource("g_Texture", g_texture, 0);

	// 设置渲染状态
	NNN::State::SetRenderState(g_render_state);

	// 设置采样状态
	//NNN::State::SetSamplerState(g_sampler_state, 0, 0, g_texture);

	//NNN::Device::DeviceContext::SetEffect(effect, NNN_SHADER_LIBS_TEXTURE_COLORTEXTURE_DX9_TECH_NAME);
	//NNN::Device::DeviceContext::DrawIndexed(6, 0, 0, 4);
	//NNN::Device::DeviceContext::Draw(4);
	//NNN::Device::DeviceContext::EndEffect();

	frame++;
	g_World = DirectX::XMMatrixTranslation(ddx, ddy, ddz);
	//DirectX::XMMATRIX g_offset= DirectX::XMMatrixTranslation(-10,-10.5,0);
	DirectX::XMMATRIX g_scale = DirectX::XMMatrixScaling(0.05, 0.05, 0.05);
	DirectX::XMMATRIX g_tmpRot;
	
	switch (mode % 3)
	{
	case 0:
		g_tmpRot = DirectX::XMMatrixRotationX(-0.01*3.141592*frame);
		break;
	case 1:
		g_tmpRot = DirectX::XMMatrixRotationY(-0.01*3.141592*frame);
		break;
	case 2:
		g_tmpRot = DirectX::XMMatrixRotationZ(-0.01*3.141592*frame);
		break;
	}
	mWVP = g_scale * g_World  * mRot * g_View * g_Projection;
	effect->SetMatrix("g_mWVP", (const float*)&mWVP);
	
#ifdef FastHack
	if (!lock)
#endif
	character->Render(0,-1,g_effect);
	
	g_World = DirectX::XMMatrixTranslation(0, 0, 0);
	
	mWVP = g_World * mRot * g_View * g_Projection;
	effect->SetMatrix("g_mWVP", (const float*)&mWVP);

	inst->Render(0,-1,g_effect);
	
	rect->Render(g_effect);

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
	//inst->GetMap()->LoadFromFile(L".\\DefaultMap.tmp");
	//return;

	inst->GetMap()->SetBlock(0, 0, 0, 0);

	inst->GetMap()->SetBlock(1, 0, 0, 0);
	inst->GetMap()->SetBlock(2, 0, 0, 0);
	inst->GetMap()->SetBlock(3, 0, 0, 0);

	inst->GetMap()->SetBlock(0, 1, 0, 0);
	inst->GetMap()->SetBlock(0, 2, 0, 0);
	inst->GetMap()->SetBlock(0, 3, 0, 0);

	inst->GetMap()->SetBlock(0, 0, 1, 0);
	inst->GetMap()->SetBlock(0, 0, 2, 0);
	inst->GetMap()->SetBlock(0, 0, 3, 0);
	int i;
	
	for (i = 0; i < 5000; i++)
	{
		//inst->GetMap()->SetBlock(sampler(), sampler(), sampler(), 1);
	}
	
}



void Init_Effect()
{
	NNN::Shader::c_Effect::s_init_param_info param_info_vs[] =
	{
		{ { "g_mWVP", NNN::Buffer::s_ConstantBuffer::es_ParamType::Matrix, 1, 0 }, 0 },
		//{ { "g_Light", NNN::Buffer::s_ConstantBuffer::es_ParamType::Vector, 1, 64 }, 0 },
	};

	std::vector<struct NNN::Shader::c_Effect::s_init_param_info> params_vs;
	for (int i = 0; i < _countof(param_info_vs); ++i)
		params_vs.push_back(param_info_vs[i]);

	NNN::Shader::c_Effect::s_init_param_info param_info_ps[] =
	{
		{ { "g_LightDir", NNN::Buffer::s_ConstantBuffer::es_ParamType::Vector, 1, 0 }, 0 },
		{ { "g_Light", NNN::Buffer::s_ConstantBuffer::es_ParamType::Vector, 1, 16 }, 0 },
	};

	std::vector<struct NNN::Shader::c_Effect::s_init_param_info> params_ps;
	for (int i = 0; i < _countof(param_info_ps); ++i)
		params_ps.push_back(param_info_ps[i]);

	const WCHAR k_EFFECT_NAME[] = L"DefaultTech";
	NNN::Shader::Add_Effect_DX11(k_EFFECT_NAME,
		L"Shaders/dx11.vs.hlslo", &params_vs,
		L"Shaders/dx11.ps.hlslo", &params_ps);
	g_effect = NNN::Shader::Find_Effect(k_EFFECT_NAME);

	g_layout = new struct NNN::Shader::s_InputLayout();
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};
	g_layout->Init_DX11(layout, _countof(layout), g_effect->DX11.m_vs);
}

/*==============================================================
 * 初始化函数
 * OnCreate_func()
 *==============================================================*/
void OnCreate_func(void* /*pUserContext*/)
{
	HRESULT  hr;
	// TODO: 初始化代码

	DirectX::XMVECTOR vecEye = { 5.0f, 5.0f,5.0f };
	DirectX::XMVECTOR vecAt = { 0.0f, 0.0f, 0.0f };

	g_cam.SetViewParams(vecEye, vecAt);
	DirectX::XMFLOAT3 vMin = { -1000.0f, -1000.0f, -1000.0f };
	DirectX::XMFLOAT3 vMax = { 1000.0f, 1000.0f, 1000.0f };

	g_cam.SetRotateButtons(FALSE, FALSE, FALSE);
	g_cam.SetClipToBoundary(TRUE, &vMin, &vMax);
	g_cam.FrameMove(0);

	//g_cam.Set

	/*
	{
		DirectX::XMVECTOR vec = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(-1.0625, 0.0625, 1.125, -0.0625));
		DirectX::XMFLOAT4 tv;
		DirectX::XMStoreFloat4(&tv,DirectX::XMVectorOrInt(DirectX::XMVectorAndInt(vec, DirectX::XMVectorSplatSignMask()),DirectX::XMVectorSplatOne()));
		DirectX::XMStoreFloat4(&tv,DirectX::XMVector3Normalize(vec));
		//
	}*/

	Init_Effect();

	g_World = DirectX::XMMatrixTranslation(0, 0, 0);

	V(NNN::Texture::Add(L"Texture_Default.png",L"Texture_Default.png", 0xffff00ff, true));

	inst = new Nyan::Scene(&g_allocator);
	character = new Nyan::Scene(&g_allocator);
	rect = new Nyan::SelectRect(&g_allocator);

	inst->SetGroundVisiablity(true);
	character->SetGroundVisiablity(false);

	inst->LoadScene(L"DefaultMap.scfg");
	character->LoadScene(L"Character.scfg");
	//ResetMap();
	//inst->InitBuffer();

	/*
	character->InitScene(22, 22, 22);
	inst->InitScene(128,128,60);
	
	character->ImportTexture(L"Texture0.png");
	character->ImportTexture(L"Texture1.png");
	character->ImportTexture(L"Texture2.png");
	character->ImportTexture(L"Texture3.png");
	character->ImportTexture(L"Texture4.png");
	character->ImportTexture(L"Texture5.png");
	character->ImportTexture(L"Texture6.png");
	character->ImportTexture(L"Texture7.png");
	character->ImportTexture(L"Texture8.png");
	character->ImportTexture(L"Texture9.png");
	character->ImportTexture(L"Texture10.png");
	character->ImportTexture(L"Texture11.png");
	character->ImportTexture(L"Texture12.png");
	character->ImportTexture(L"Texture13.png");
	character->ImportTexture(L"Texture14.png");
	character->ImportTexture(L"Texture15.png");
	character->ImportTexture(L"Texture16.png");

	inst->ImportTexture(L"Texture0.png");
	inst->ImportTexture(L"Texture1.png");
	inst->ImportTexture(L"Texture2.png");
	inst->ImportTexture(L"Texture3.png");
	inst->ImportTexture(L"Texture4.png");
	inst->ImportTexture(L"Texture5.p\ng");
	inst->ImportTexture(L"Texture6.png");
	inst->ImportTexture(L"Texture7.png");
	inst->ImportTexture(L"Texture8.png");
	inst->ImportTexture(L"Texture9.png");
	inst->ImportTexture(L"Texture10.png");
	inst->ImportTexture(L"Texture11.png");
	inst->ImportTexture(L"Texture12.png");
	inst->ImportTexture(L"Texture13.png");
	inst->ImportTexture(L"Texture14.png");
	inst->ImportTexture(L"Texture15.png");
	inst->ImportTexture(L"Texture16.png");
	*/

#ifdef FastHack
	auto setfunc = [](float* change, float* target,const int& self,Nyan::TimerManage* inst)
	{
		if (*target > 0)
		{
			if (fabs(*change - *target) > 0.05)
			{
				if (*change < *target)
				{
					*change += 0.05;
				}
				else
				{
					*change -= 0.05;
				}
			}
			else
			{
				*change = *target;
			}
		}
	};
	auto func1 = std::bind(setfunc, &ddx, &tdx, _1, _2);
	auto func2 = std::bind(setfunc, &ddy, &tdy, _1, _2);
	auto func3 = std::bind(setfunc, &ddz, &tdz, _1, _2);
	Tim.StartTimer(Tim.SetTimer(func1, 1, true));
	Tim.StartTimer(Tim.SetTimer(func2, 1, true));
	Tim.StartTimer(Tim.SetTimer(func3, 1, true));
#endif

	//ResetMap();

	
 	//inst->InitBuffer();

	//character->GetMap()->LoadFromFile(L"chr_jp.tmp");
	
	//character->InitBuffer();

	ctex = 0;
	rect->SetUV(inst->GetPiece(ctex)->m_min_u, inst->GetPiece(ctex)->m_max_u, inst->GetPiece(ctex)->m_min_v, inst->GetPiece(ctex)->m_max_v);
	Init_Buffer();
	Buffer_FillData();

	Adapter = new Nyan::Scene2RayAdapter(inst);
	
	//g_View = NNN::Misc::GetOrthoView();
	
	//V(NNN::Texture::Add(L"Character.png", L"Character.png", 0xffff00ff, true));
;





	




	ChangeSize(NNN::Misc::GetClientSize().cx, NNN::Misc::GetClientSize().cy);

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
	delete character;
	delete rect;

	SAFE_RELEASE(p_ByteAddrBufferView);
	SAFE_RELEASE(p_ByteAddrBuffer);

	SAFE_RELEASE(g_layout);
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
		g_Projection = NNN::Misc::GetPerspectiveLH((int)width, (int)height);
}

int WriteToPNG_Gray(TCHAR* Filepath, ImgFormat** Info, int width, int height)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;

	if ((_tfopen_s(&fp, Filepath, _T("wb"))) != 0)
	{
		return 1;
	}
	png_ptr = png_create_write_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_ptr, info_ptr);
	png_write_image(png_ptr, Info);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	return 0;
}

int WriteToPNG_Color(TCHAR* Filepath, ImgFormat** Info, int width, int height)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;

	if ((_tfopen_s(&fp, Filepath, _T("wb"))) != 0)
	{
		return 1;
	}
	png_ptr = png_create_write_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_ptr, info_ptr);
	png_write_image(png_ptr, Info);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	return 0;
}


#define Info_Resolution_X 600
#define Info_Resolution_Y 600

void BuildRayTracePic()
{
	ImgFormat** pointer;
	ImgFormat** pointer2;
	//Solution Info_Resolution_X*Info_Resolution_Y
	pointer = new ImgFormat*[Info_Resolution_X];
	pointer2 = new ImgFormat*[Info_Resolution_X];
	for (int i = 0; i < Info_Resolution_X; ++i)
	{
		pointer[i] = new ImgFormat[Info_Resolution_Y*3];
		pointer2[i] = new ImgFormat[Info_Resolution_Y];
		memset(pointer[i], 0, sizeof(ImgFormat)*Info_Resolution_Y*3);
		memset(pointer2[i], 0, sizeof(ImgFormat)*Info_Resolution_Y);
	}
	XMFLOAT4 eyept;
	XMFLOAT4 dir;
	XMFLOAT4 up;

	up.x = 1;
	up.y = up.z = 0;
	Nyan::LineFunc *func;
	{
		DirectX::XMVECTOR m_mouse = DirectX::XMVectorSet(0, 0, 0, 1);
		DirectX::XMVECTOR m_mouse1 = DirectX::XMVectorSet(((float)NNN::Input::Mouse::MouseX() * 2 / NNN::Misc::GetClientSize(false).cx - 1) / g_Projection.r[0].m128_f32[0], 
			-((float)NNN::Input::Mouse::MouseX() * 2 / NNN::Misc::GetClientSize(false).cy - 1) / g_Projection.r[1].m128_f32[1], 1, 0);
		DirectX::XMMATRIX viewinv = DirectX::XMMatrixInverse(nullptr, g_cam.GetViewMatrix());
		DirectX::XMMATRIX worinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(0, 0, 0));
		DirectX::XMMATRIX rotinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationX(-0.5 * 3.141592));

		m_mouse = DirectX::XMVector4Transform(m_mouse, viewinv);
		m_mouse = DirectX::XMVector4Transform(m_mouse, rotinv);
		m_mouse = DirectX::XMVector4Transform(m_mouse, worinv);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, viewinv);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, rotinv);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, worinv);

		DirectX::XMFLOAT4 ori, dir;
		DirectX::XMStoreFloat4(&ori, m_mouse);
		DirectX::XMStoreFloat4(&dir, m_mouse1);
		func =new  Nyan::LineFunc(ori, dir);
	}
	struct dat
	{
		ImgFormat** a;
		ImgFormat** b;
	};
	dat picinfo;
	picinfo.a = pointer;
	picinfo.b = pointer2;
	//int result = WriteToPNG_Color(_T("RayResult.png"), pointer, Info_Resolution_X, Info_Resolution_Y);
	//XMStoreFloat4(&dir, XMVector4Transform(XMVector3Normalize(XMVectorSubtract(g_cam.GetLookAtPt(), g_cam.GetEyePt())), DirectX::XMMatrixRotationX(0.5*3.141592)));
	//XMStoreFloat4(&eyept, XMVector4Transform(g_cam.GetEyePt(),DirectX::XMMatrixRotationX(0.5*3.141592)));
	(*Adapter)().GenerateRaytraceResult(Info_Resolution_X, Info_Resolution_Y, 0.0005, func->p1, 4, 4000, func->n, up,
		[](int x,int y,const int&TexType,const XMFLOAT4& loc,const XMFLOAT4& rn,const float&depth,void *arg)
	{
		ImgFormat** p = (ImgFormat**)arg;
		((dat*)arg)->a[x][y * 3] = (rn.x+1)*127;
		((dat*)arg)->a[x][y * 3 + 1] = (rn.y + 1) * 127;
		((dat*)arg)->a[x][y * 3 + 2] = (rn.z + 1) * 127;
		((dat*)arg)->b[x][y] = (depth < 254 ? depth : 254) + 1;
	}, &picinfo);
	WriteToPNG_Color(_T("RayResult.png"), pointer, Info_Resolution_X, Info_Resolution_Y);
	WriteToPNG_Gray(_T("RayResult_depth.png"), pointer2, Info_Resolution_X, Info_Resolution_Y);
}


/*==============================================================
 * 处理按键
 * OnKeyboard()
 *
 *		nChar		- 按键（例如：'A'、'a'、VK_LEFT、VK_RIGHT）
 *		bKeyDown	- 是否 WM_KEYDOWN 或 WM_SYSKEYDOWN
 *		bAltDown	- Alt 是否按下
 *==============================================================*/
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool /*bAltDown*/, void* /*pUserContext*/)
{
	static bool onceflag_I = false, onceflag_R = false;
	if (!bKeyDown)
	{
		return;
	}
	if (nChar == '5')
	{
		lock = !lock;
		if (lock){
			DirectX::XMVECTOR vecEye = { ddx + 0.5, ddz + 0.7, -ddy - 0.5 };
			DirectX::XMVECTOR vecatori = DirectX::XMVectorSubtract(g_cam.GetLookAtPt(), g_cam.GetEyePt());
			vecatori = DirectX::XMVectorAdd(vecEye, vecatori);

			g_cam.SetViewParams(vecEye, vecatori);
		}
	}
#ifdef FastHack
	if (nChar == '0')
	{
		lightdist += 10;
	}
	if (nChar == '1')
	{
		lightdist -= 10;
	}
#endif
	if (nChar == 'F' || nChar == 'f')
	{
		inst->GetMap()->SaveToFile(L".\\DefaultMap.tmp");
	}
	if (nChar == 'L' || nChar == 'l')
	{
		if(tdy<inst->GetMap()->GetY()-1)tdy++;
	}
	if (nChar == 'K' || nChar == 'k')
	{
		if(tdy>0)tdy--;
	}
	if (nChar == 'Y' || nChar == 'y')
	{
		mode++;
	}
	if (nChar == 'O' || nChar == 'o')
	{
		if(tdx>0)tdx--;
	}
	if (nChar == 'P' || nChar == 'p')
	{
		if (tdy<inst->GetMap()->GetX() - 1)tdx++;
	}
	if (nChar == 'm' || nChar == 'M')
	{
		DirectX::XMVECTOR m_mouse = DirectX::XMVectorSet(0, 0, 0, 1);
		DirectX::XMVECTOR m_mouse1 = DirectX::XMVectorSet((
			(float)NNN::Input::Mouse::MouseX() * 2 / NNN::Misc::GetClientSize(false).cx - 1) /
			g_Projection.r[0].m128_f32[0],
			-((float)NNN::Input::Mouse::MouseY() * 2 / NNN::Misc::GetClientSize(false).cy - 1) /
			g_Projection.r[1].m128_f32[1], 1, 0);
		DirectX::XMMATRIX viewinv = DirectX::XMMatrixInverse(nullptr, g_cam.GetViewMatrix());
		DirectX::XMMATRIX worinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(0, 0, 0));
		DirectX::XMMATRIX rotinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationX(-0.5 * 3.141592));

		m_mouse = DirectX::XMVector4Transform(m_mouse, viewinv);
		m_mouse = DirectX::XMVector4Transform(m_mouse, rotinv);
		m_mouse = DirectX::XMVector4Transform(m_mouse, worinv);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, viewinv);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, rotinv);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, worinv);

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
			tdx = nx;
			tdy = ny;
			tdz = nz;
			if (tdz == -1)
			{
				++tdz;
			}
			/*
			timers.clear();
			timers.push_back(std::make_pair(std::bind(
				[](float* target, float targetval, void* ex, bool islast, int curcnt)
					{
							
					}
				,(&ddx),float(nx),(void*)(new int(120)),std::placeholders::_1,std::placeholders::_2), 120));
					//此处有内存泄漏,泄漏一个int,之后再进行修正
			*/
		}
	}
	if (nChar == 'Z' || nChar == 'z')
	{
		//Build Raytrace Image
		BuildRayTracePic();
	}
	if (nChar == 'Q' || nChar == 'q')
	{
		ctex = (ctex + 1) % inst->GetMap()->GetT();
		rect->SetUV(inst->GetPiece(ctex)->m_min_u, inst->GetPiece(ctex)->m_max_u, inst->GetPiece(ctex)->m_min_v, inst->GetPiece(ctex)->m_max_v);
	}
	if (nChar == 'I' || nChar == 'i' || nChar == 'E' || nChar == 'E')//(NNN::Input::Keyboard::isKeyDown(DIK_I))
	{
		if (!onceflag_I)
		{
			//onceflag_I = true;
			{
				DirectX::XMVECTOR m_mouse = DirectX::XMVectorSet(0, 0, 0, 1);
				DirectX::XMVECTOR m_mouse1 = DirectX::XMVectorSet((
					(float)NNN::Input::Mouse::MouseX() * 2 / NNN::Misc::GetClientSize(false).cx - 1) /
					g_Projection.r[0].m128_f32[0],
					-((float)NNN::Input::Mouse::MouseY() * 2 / NNN::Misc::GetClientSize(false).cy - 1) /
					g_Projection.r[1].m128_f32[1], 1, 0);
				DirectX::XMMATRIX viewinv = DirectX::XMMatrixInverse(nullptr, g_cam.GetViewMatrix());
				DirectX::XMMATRIX worinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(0, 0, 0));
				DirectX::XMMATRIX rotinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationX(-0.5 * 3.141592));

				m_mouse = DirectX::XMVector4Transform(m_mouse, viewinv);
				m_mouse = DirectX::XMVector4Transform(m_mouse, rotinv);
				m_mouse = DirectX::XMVector4Transform(m_mouse, worinv);
				m_mouse1 = DirectX::XMVector4Transform(m_mouse1, viewinv);
				m_mouse1 = DirectX::XMVector4Transform(m_mouse1, rotinv);
				m_mouse1 = DirectX::XMVector4Transform(m_mouse1, worinv);

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
						//inst->GetMap()->ReCalcBlock(nx, ny, nz);// , inst->GetMap()->At(nx, ny, nz).TexType, 0);
						//inst->GetMap()->At(nx, ny, nz).TexType = 0;
						inst->GetMap()->SetBlock(nx, ny, nz, ctex);
						inst->InitBuffer();
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

	if (nChar == 'R' || nChar == 'r')//(NNN::Input::Keyboard::isKeyDown(DIK_R))
	{
		if (!onceflag_R)
		{
			//onceflag_R = true;
			{
				DirectX::XMVECTOR m_mouse = DirectX::XMVectorSet(0, 0, 0, 1);
				DirectX::XMVECTOR m_mouse1 = DirectX::XMVectorSet((
					(float)NNN::Input::Mouse::MouseX() * 2 / NNN::Misc::GetClientSize(false).cx - 1) /
					g_Projection.r[0].m128_f32[0],
					-((float)NNN::Input::Mouse::MouseY() * 2 / NNN::Misc::GetClientSize(false).cy - 1) /
					g_Projection.r[1].m128_f32[1], 1, 0);
				DirectX::XMMATRIX viewinv = DirectX::XMMatrixInverse(nullptr, g_cam.GetViewMatrix());
				DirectX::XMMATRIX worinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(0, 0, 0));
				DirectX::XMMATRIX rotinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationX(-0.5 * 3.141592));

				m_mouse = DirectX::XMVector4Transform(m_mouse, viewinv);
				m_mouse = DirectX::XMVector4Transform(m_mouse, rotinv);
				m_mouse = DirectX::XMVector4Transform(m_mouse, worinv);
				m_mouse1 = DirectX::XMVector4Transform(m_mouse1, viewinv);
				m_mouse1 = DirectX::XMVector4Transform(m_mouse1, rotinv);
				m_mouse1 = DirectX::XMVector4Transform(m_mouse1, worinv);

				DirectX::XMFLOAT4 ori, dir;
				DirectX::XMStoreFloat4(&ori, m_mouse);
				DirectX::XMStoreFloat4(&dir, DirectX::XMVector4Normalize(m_mouse1));
				DirectX::XMFLOAT4 result = inst->TestCollisoin(Nyan::LineFunc(ori, dir));
				//rect->SetRectLocation(-1, -1, -1, -1);
				if (result.x != -1)
				{
					//rect->SetRectLocation(result.x, result.y, result.z, result.w);
					if (result.z!=-1 && inst->GetMap()->At(result.x, result.y, result.z).TexType != -1)
					{
						//inst->GetMap()->ReCalcBlock(result.x, result.y, result.z);// , inst->GetMap()->At(result.x, result.y, result.z).TexType, -1);
						inst->GetMap()->SetBlock(result.x, result.y, result.z, - 1);
						//inst->GetMap()->ClearMask();
						inst->InitBuffer();
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
#ifdef FastHack
	if (lock){
		DirectX::XMVECTOR vecEye = { ddx + 0.5, ddz + 0.7, -ddy - 0.5 };
		DirectX::XMVECTOR vecatori = DirectX::XMVectorSubtract(g_cam.GetLookAtPt(), g_cam.GetEyePt());
		vecatori = DirectX::XMVectorAdd(vecEye, vecatori);

		g_cam.SetViewParams(vecEye, vecatori);
	}
	
	//setfunc(ddx, tdx);
	//setfunc(ddy, tdy);
	//setfunc(ddz, tdz);
#endif
	Tim.Tick();
	dz = NNN::Input::Mouse::MouseZ_Delta();
	if (dz != 0)
	{
		float scale = 1.0 + (float)dz / 1200;
		//DirectX::XMVECTOR m_pos = DirectX::XMVectorSubtract(g_CamPos, g_CamFocus);
		//m_pos = DirectX::XMVectorScale(m_pos, scale);
		//g_CamPos = DirectX::XMVectorAdd(g_CamFocus, m_pos);
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
		DirectX::XMVECTOR m_mouse = DirectX::XMVectorSet(0, 0, 0, 1);
		DirectX::XMVECTOR m_mouse1 = DirectX::XMVectorSet(((float)m_lx * 2 / NNN::Misc::GetClientSize(false).cx - 1) / g_Projection.r[0].m128_f32[0], -((float)m_ly * 2 / NNN::Misc::GetClientSize(false).cy - 1) / g_Projection.r[1].m128_f32[1], 1, 0);
		DirectX::XMMATRIX viewinv = DirectX::XMMatrixInverse(nullptr, g_cam.GetViewMatrix());
		DirectX::XMMATRIX worinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(0, 0, 0));
		DirectX::XMMATRIX rotinv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationX(-0.5 * 3.141592));

		m_mouse = DirectX::XMVector4Transform(m_mouse, viewinv);
		m_mouse = DirectX::XMVector4Transform(m_mouse, rotinv);
		m_mouse = DirectX::XMVector4Transform(m_mouse, worinv);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, viewinv);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, rotinv);
		m_mouse1 = DirectX::XMVector4Transform(m_mouse1, worinv);

		DirectX::XMFLOAT4 ori, dir;
		DirectX::XMStoreFloat4(&ori, m_mouse);
		DirectX::XMStoreFloat4(&dir, m_mouse1);
		//ResetMap();
		Nyan::LineFunc line(ori, dir);
		DirectX::XMVECTOR re;
		DirectX::XMVECTOR rn;
		float depth;
		DirectX::XMFLOAT4 result;// = inst->TestCollisoin(Nyan::LineFunc(ori, dir));
		//DirectX::XMFLOAT4 tmpdir;// = inst->GetIntersect(Nyan::LineFunc(ori, dir));
		if ((*Adapter)().CalcIntersect(DirectX::XMLoadFloat4(&line.p1), DirectX::XMLoadFloat4(&line.n), &rn, &re, &depth))
		{
			DirectX::XMStoreFloat4(&result, re);
			LightConeDir = result;
		}
		DirectX::XMStoreFloat4(&result, re);
		rect->SetRectLocation(-1, -1, -1, -1);
		if (result.x >= 0)
		{
			
			rect->SetRectLocation(floor(result.x),floor(result.y),floor(result.z),(*Adapter)().n2Dir(rn));
			//inst->GetMap()->SetBlock(result.x, result.y, result.z, 2);
		}
		//inst->GetMap()->CalcMask();
		//inst->GetMap()->CountRect();
		//inst->InitBuffer();
		//{result.x},{result.y},{result.z},{result.w}\n
	}
	//static float p_x, p_y;
	if (NNN::Input::Mouse::isMouseButtonDown(1))
	{
		g_cam.RotationYawPitch((float)dx / 200, (float)dy / 200);
	}
	else if (NNN::Input::Mouse::isMouseButtonDown(0))
	{

		//g_cam.TranslationYawPitchRoll( 0, 0, (float)-dy / 10);
		g_cam.RotationYawPitch( (float)dx / 200, 0);
		//g_cam.TranslationYawPitchRoll((float)dx/10, 0, 0);
		g_cam.TranslationXYZ(0, 0, (float)dy / 10);
	}
	if (NNN::Input::Keyboard::isKeyDown(DIK_W) || NNN::Input::Keyboard::isKeyDown(DIK_UP))
	{
		g_cam.TranslationYawPitchRoll(0, 0, (float)0.05);
	}
	if (NNN::Input::Keyboard::isKeyDown(DIK_S) || NNN::Input::Keyboard::isKeyDown(DIK_DOWN))
	{
		g_cam.TranslationYawPitchRoll(0, 0, (float)-0.05);
	}
	if (NNN::Input::Keyboard::isKeyDown(DIK_A) || NNN::Input::Keyboard::isKeyDown(DIK_LEFT))
	{
		g_cam.TranslationYawPitchRoll((float)-0.05,0,0);
	}
	if (NNN::Input::Keyboard::isKeyDown(DIK_D) || NNN::Input::Keyboard::isKeyDown(DIK_RIGHT))
	{
		g_cam.TranslationYawPitchRoll((float)0.05,0,0);
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
		//L"Nyan.whp",
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
