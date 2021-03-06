﻿//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//
// Desc : 主程序的头文件
//--------------------------------------------------------------------------------------

#pragma once
#ifndef _NYAN___NYAN_H_
#define _NYAN___NYAN_H_



#include "../../../Src/nnnEngine/nnn.h"



// OpenGL 部分
void	CALLBACK OpenGL_Init( void *pUserContext );
void	CALLBACK OpenGL_Final( void *pUserContext );
void	CALLBACK OpenGL_ChangeSize( GLsizei w, GLsizei h, void* pUserContext );
void	CALLBACK OpenGL_OnFrameRender( double fTime, float fElapsedTime, void* pUserContext );

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
// DirectX 9 部分
bool	CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT	CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT	CALLBACK OnD3D9ResetDevice(	IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void	CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void	CALLBACK OnD3D9LostDevice( void* pUserContext );
void	CALLBACK OnD3D9DestroyDevice( void* pUserContext );
#endif	// NNN_PLATFORM_WIN32

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)
// DirectX 11 部分
bool	CALLBACK IsD3D11DeviceAcceptable( const NNN::CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const NNN::CD3D11EnumDeviceInfo* DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
HRESULT	CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT	CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void	CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext );
#else	// WP8
HRESULT	CALLBACK OnD3D11CreateDevice( ID3D11Device1* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT	CALLBACK OnD3D11ResizedSwapChain( ID3D11Device1* pd3dDevice, IDXGISwapChain1* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void	CALLBACK OnD3D11FrameRender( ID3D11Device1* pd3dDevice, ID3D11DeviceContext1* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext );
#endif	// NNN_PLATFORM_WIN32
void	CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void	CALLBACK OnD3D11DestroyDevice( void* pUserContext );
#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8

//==================== 公共函数（只是抽出来放在主文件、不放在各个 API 文件中，方便编写） ====================(Start)
// 通用渲染函数
HRESULT	Render(double fTime, float fElapsedTime, void* pUserContext);

// 初始化/清理
void	OnCreate_func(void* pUserContext);
void	OnDestroy_func(void* pUserContext);

// Reset/Lost
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
HRESULT	OnReset_dx9(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void	OnLost_dx9(void* pUserContext);
#endif	// NNN_PLATFORM_WIN32

// 改变分辨率大小
void	ChangeSize(UINT width, UINT height);
//==================== 公共函数（只是抽出来放在主文件、不放在各个 API 文件中，方便编写） ====================(End)



#endif	/* _NYAN___NYAN_H_ */
