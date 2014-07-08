//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//
// Desc : OpenGL 部分
//--------------------------------------------------------------------------------------

#include "../../../Src/nnnEngine/nnn.h"
#include "Nyan.h"

#if (NNN_PLATFORM != NNN_PLATFORM_WP8)
/*==============================================================
 * OpenGL 初始化
 * OpenGL_Init()
 *==============================================================*/
void CALLBACK OpenGL_Init( void* pUserContext )
{
	OnCreate_func(pUserContext);
}


/*==============================================================
 * OpenGL 清理
 * OpenGL_Final()
 *==============================================================*/
void CALLBACK OpenGL_Final( void* pUserContext )
{
	OnDestroy_func(pUserContext);
}


/*==============================================================
 * OpenGL 改变窗口大小
 * OpenGL_ChangeSize()
 *==============================================================*/
void CALLBACK OpenGL_ChangeSize( GLsizei w, GLsizei h, void* /*pUserContext*/ )
{
	ChangeSize((UINT)w, (UINT)h);
}


/*==============================================================
 * OpenGL 渲染场景
 * OpenGL_OnFrameRender()
 *==============================================================*/
void CALLBACK OpenGL_OnFrameRender( double fTime, float fElapsedTime, void* pUserContext )
{
	Render(fTime, fElapsedTime, pUserContext);
}
#endif	// !NNN_PLATFORM_WP8
