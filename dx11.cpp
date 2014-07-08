//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//
// Desc : DirectX 11 部分
//--------------------------------------------------------------------------------------

#include "../../../Src/nnnEngine/nnn.h"
#include "Nyan.h"

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
/*==============================================================
 * 设备能力检查。如果任何一项返回 false ，则拒绝 HAL 设备
 * IsD3D11DeviceAcceptable()
 *==============================================================*/
bool CALLBACK IsD3D11DeviceAcceptable(	const NNN::CD3D11EnumAdapterInfo* /*AdapterInfo*/, UINT /*Output*/,
										const NNN::CD3D11EnumDeviceInfo* /*DeviceInfo*/,
										DXGI_FORMAT /*BackBufferFormat*/, bool /*bWindowed*/, void* /*pUserContext*/ )
{
	return true;
}


/*==============================================================
 * 创建不依赖于后台缓冲区的任何 D3D11 资源
 * OnD3D11CreateDevice()
 *==============================================================*/
HRESULT CALLBACK OnD3D11CreateDevice(
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
										ID3D11Device* /*pd3dDevice*/,
#else	// WP8
										ID3D11Device1* /*pd3dDevice*/,
#endif	// NNN_PLATFORM_WIN32
										const DXGI_SURFACE_DESC* /*pBackBufferSurfaceDesc*/, void* pUserContext )
{
	OnCreate_func(pUserContext);
	return S_OK;
}


/*==============================================================
 * 创建依赖于后台缓冲区的任何 D3D11 资源
 * OnD3D11ResizedSwapChain()
 *==============================================================*/
HRESULT CALLBACK OnD3D11ResizedSwapChain(
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
											ID3D11Device* /*pd3dDevice*/, IDXGISwapChain* /*pSwapChain*/,
#else	// WP8
											ID3D11Device1* /*pd3dDevice*/, IDXGISwapChain1* /*pSwapChain*/,
#endif	// NNN_PLATFORM_WIN32
											const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* /*pUserContext*/ )
{
	UINT width	= pBackBufferSurfaceDesc->Width;
	UINT height	= pBackBufferSurfaceDesc->Height;

#if (NNN_PLATFORM == NNN_PLATFORM_WP8)
	switch(NNN::Input::Rotation::GetRotation())
	{
	case NNN::Input::Rotation::es_Rotation::Rotate90:
	case NNN::Input::Rotation::es_Rotation::Rotate270:
		std::swap(width, height);
		break;
	}
#endif	// NNN_PLATFORM_WP8

	ChangeSize(width, height);
	return S_OK;
}


/*==============================================================
 * 使用 D3D11 设备渲染场景
 * OnD3D11FrameRender()
 *==============================================================*/
void CALLBACK OnD3D11FrameRender(	
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
									ID3D11Device* /*pd3dDevice*/, ID3D11DeviceContext* /*pd3dImmediateContext*/,
#else	// WP8
									ID3D11Device1* /*pd3dDevice*/, ID3D11DeviceContext1* /*pd3dImmediateContext*/,
#endif	// NNN_PLATFORM_WIN32
									double fTime, float fElapsedTime, void* pUserContext )
{
	Render(fTime, fElapsedTime, pUserContext);
}


/*==============================================================
 * 释放在 OnD3D11ResizedSwapChain() 中创建的 D3D11 资源
 * OnD3D11ReleasingSwapChain()
 *==============================================================*/
void CALLBACK OnD3D11ReleasingSwapChain( void* /*pUserContext*/ )
{
}


/*==============================================================
 * 释放在 OnD3D11CreateDevice() 中创建的 D3D11 资源
 * OnD3D11DestroyDevice()
 *==============================================================*/
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	OnDestroy_func(pUserContext);
}
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8
