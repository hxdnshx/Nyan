//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//
// Desc : DirectX 9 部分
//--------------------------------------------------------------------------------------

#include "../../../Src/nnnEngine/nnn.h"
#include "Nyan.h"

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
/*==============================================================
 * 设备能力检查。如果任何一项返回 false，则拒绝 HAL 设备
 * IsD3D9DeviceAcceptable()
 *==============================================================*/
bool CALLBACK IsD3D9DeviceAcceptable(	D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
										bool /*bWindowed*/, void* /*pUserContext*/ )
{
	// Typically want to skip back buffer formats that don't support alpha blending
	IDirect3D9* pD3D = NNN::DXUTGetD3D9Object();
	if( FAILED( pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
										AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
										D3DRTYPE_TEXTURE, BackBufferFormat) ) )
		return false;

	// No fallback defined by this app, so reject any device that doesn't support at least ps2.0
	if( pCaps->PixelShaderVersion < D3DPS_VERSION( 2, 0 ) )
		return false;

	return true;
}


/*==============================================================
 * 创建任何 D3D9 的托管资源(D3DPOOL_MANAGED)
 * OnD3D9CreateDevice()
 *==============================================================*/
HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* /*pd3dDevice*/, const D3DSURFACE_DESC* /*pBackBufferSurfaceDesc*/,
									void* pUserContext)
{
	OnCreate_func(pUserContext);
	return S_OK;
}


/*==============================================================
 * 创建任何 D3D9 的默认资源(D3DPOOL_DEFAULT)
 * OnD3D9ResetDevice()
 *==============================================================*/
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
									void* pUserContext )
{
	ChangeSize(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);

	HRESULT hr;
	V_RETURN( OnReset_dx9(pd3dDevice, pBackBufferSurfaceDesc, pUserContext) );

	return S_OK;
}


/*==============================================================
 * 使用 D3D9 设备渲染场景
 * OnD3D9FrameRender()
 *==============================================================*/
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	HRESULT hr;

	if( SUCCEEDED(pd3dDevice->BeginScene()) )
	{
		Render(fTime, fElapsedTime, pUserContext);

		V( pd3dDevice->EndScene() );
	}
}


/*==============================================================
 * 释放在 OnD3D9ResetDevice() 中创建的对象
 * OnD3D9LostDevice()
 *==============================================================*/
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
	OnLost_dx9(pUserContext);
}


/*==============================================================
 * 释放在 OnD3D9CreateDevice() 中创建的对象
 * OnD3D9DestroyDevice()
 *==============================================================*/
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
	OnDestroy_func(pUserContext);
}
#endif	// NNN_PLATFORM_WIN32
