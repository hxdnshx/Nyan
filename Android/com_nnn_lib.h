//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//
// Desc : Android JNI
//--------------------------------------------------------------------------------------

#pragma once
#ifndef _JNI___COM_NNN_LIB_H_
#define _JNI___COM_NNN_LIB_H_

#include "../../../../Src/common/common-macro.h"

#if (NNN_PLATFORM == NNN_PLATFORM_ANDROID)

#include <jni.h>

#include "../../../../Src/common/common.h"

#define NNN_ANDROID_JNI_FUNC(func)	JNIEXPORT void Java_##com_nnn_lib_nnnLib_##func

extern "C"
{
NNN_ANDROID_JNI_FUNC(OnInit)			(JNIEnv *env, jclass c);							// 初始化
NNN_ANDROID_JNI_FUNC(OnFinal)			(JNIEnv *env, jclass c);							// 清理
NNN_ANDROID_JNI_FUNC(OnRender)			(JNIEnv *env, jclass c);							// 渲染
NNN_ANDROID_JNI_FUNC(OnSizeChanged)		(JNIEnv *env, jclass c, jint width, jint height);	// 改变大小
NNN_ANDROID_JNI_FUNC(OnPaused)			(JNIEnv *env, jclass c);							// 暂停游戏
NNN_ANDROID_JNI_FUNC(OnResume)			(JNIEnv *env, jclass c);							// 恢复游戏

NNN_ANDROID_JNI_FUNC(OnReciveSdcardPath)(JNIEnv *env, jclass c, jstring path);				// 接收 sdcard 路径
NNN_ANDROID_JNI_FUNC(OnInitAssetManager)(JNIEnv *env, jclass c, jobject assetManager);		// 初始化 AssetManager

// 初始化/清理输入法
NNN_ANDROID_JNI_FUNC(InitIME)			(JNIEnv *env, jclass c, jobject activity, jobject textbox);
NNN_ANDROID_JNI_FUNC(FinalIME)			(JNIEnv *env, jclass c);

// 触摸事件
NNN_ANDROID_JNI_FUNC(OnTouches)			(	JNIEnv *env, jclass c,
											jobjectArray touches, int touches_count, int Msg,
											int point_x, int point_y, int touch_id );

NNN_ANDROID_JNI_FUNC(OnRotation)		(JNIEnv *env, jclass c, int rotation);					// 旋转屏幕
NNN_ANDROID_JNI_FUNC(OnAccelerometer)	(JNIEnv *env, jclass c, jfloat x, jfloat y, jfloat z);	// 加速计（加速度计）
NNN_ANDROID_JNI_FUNC(OnGyrometer)		(JNIEnv *env, jclass c, jfloat x, jfloat y, jfloat z);	// 陀螺测试仪（三轴螺旋仪）
NNN_ANDROID_JNI_FUNC(OnMagnetometer)	(JNIEnv *env, jclass c, jfloat x, jfloat y, jfloat z);	// 地磁仪（磁强计、磁场感应器）
}

#endif  // NNN_PLATFORM_ANDROID

#endif  /* _JNI___COM_NNN_LIB_H_ */
