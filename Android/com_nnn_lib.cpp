//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//--------------------------------------------------------------------------------------

#include "../../../../Src/common/common-macro.h"

#if (NNN_PLATFORM == NNN_PLATFORM_ANDROID)

#include <jni.h>

#include "../../../../Src/nnnLib/nnnLib.h"
#include "../../../../Src/nnnEngine/nnn.h"

#include "com_nnn_lib.h"

/*==============================================================
 * 初始化
 * Java_com_nnn_lib_nnnLib_OnInit()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnInit) (JNIEnv* env, jclass c)
{
	NNN_ANDROID_LOG_INFO("NNN_ANDROID_JNI_FUNC(OnInit)");

	NNN::Platform::Android::JNI_OnInit(env, c);
}


/*==============================================================
 * 清理
 * Java_com_nnn_lib_nnnLib_OnFinal()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnFinal) (JNIEnv* env, jclass c)
{
	NNN_ANDROID_LOG_INFO("NNN_ANDROID_JNI_FUNC(OnFinal)");

	NNN::Platform::Android::JNI_OnFinal(env, c);
}


/*==============================================================
 * 渲染
 * Java_com_nnn_lib_nnnLib_OnRender()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnRender) (JNIEnv* env, jclass c)
{
	NNN::Platform::Android::JNI_OnRender(env, c);
}


/*==============================================================
 * 改变大小
 * Java_com_nnn_lib_nnnLib_OnSizeChanged()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnSizeChanged) (JNIEnv *env, jclass c, jint width, jint height)
{
	NNN_ANDROID_LOG_INFO("NNN_ANDROID_JNI_FUNC(OnSizeChanged) : %d * %d", width, height);

	NNN::Platform::Android::JNI_OnSizeChanged(env, c, width, height);
}


/*==============================================================
 * 暂停游戏
 * Java_com_nnn_lib_nnnLib_OnPaused()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnPaused) (JNIEnv *env, jclass c)
{
	NNN_ANDROID_LOG_INFO("NNN_ANDROID_JNI_FUNC(OnPaused)");

	NNN::Platform::Android::JNI_OnPaused(env, c);
}


/*==============================================================
 * 恢复游戏
 * Java_com_nnn_lib_nnnLib_OnResume()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnResume) (JNIEnv *env, jclass c)
{
	NNN_ANDROID_LOG_INFO("NNN_ANDROID_JNI_FUNC(OnResume)");

	NNN::Platform::Android::JNI_OnResume(env, c);
}


/*==============================================================
 * 接收 sdcard 路径
 * Java_com_nnn_lib_nnnLib_OnReciveSdcardPath()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnReciveSdcardPath) (JNIEnv *env, jclass c, jstring path)
{
	NNN::Platform::Android::JNI_OnReciveSdcardPath(env, c, path);
}


/*==============================================================
 * 初始化 AssetManager
 * Java_com_nnn_lib_nnnLib_OnInitAssetManager()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnInitAssetManager) (JNIEnv *env, jclass c, jobject assetManager)
{
	NNN::Platform::Android::JNI_OnInitAssetManager(env, c, assetManager);
}


/*==============================================================
 * 初始化/清理输入法
 * Java_com_nnn_lib_nnnLib_InitIME()
 * Java_com_nnn_lib_nnnLib_FinalIME()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(InitIME) (JNIEnv *env, jclass c, jobject activity, jobject textbox)
{
	NNN::Platform::Android::JNI_InitIME(env, c, activity, textbox);
}
//--------------------------------------------------
NNN_ANDROID_JNI_FUNC(FinalIME) (JNIEnv *env, jclass c)
{
	NNN::Platform::Android::JNI_FinalIME(env, c);
}


/*==============================================================
 * 触摸事件
 * Java_com_nnn_lib_nnnLib_OnTouches()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnTouches) (	JNIEnv *env, jclass c,
									jobjectArray touches, int touches_count, int Msg,
									int point_x, int point_y, int touch_id )
{
	NNN::Platform::Android::JNI_OnTouches(env, c, touches, touches_count, Msg, point_x, point_y, touch_id);
}


/*==============================================================
 * 旋转屏幕事件
 * Java_com_nnn_lib_nnnLib_OnRotation()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnRotation) (JNIEnv *env, jclass c, int rotation)
{
	NNN::Platform::Android::JNI_OnRotation(env, c, rotation);
}


/*==============================================================
 * 加速计（加速度计）
 * Java_com_nnn_lib_nnnLib_OnAccelerometer()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnAccelerometer) (JNIEnv *env, jclass c, jfloat x, jfloat y, jfloat z)
{
	NNN::Platform::Android::JNI_OnAccelerometer(env, c, x, y, z);
}


/*==============================================================
 * 陀螺测试仪（三轴螺旋仪）
 * Java_com_nnn_lib_nnnLib_OnGyrometer()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnGyrometer) (JNIEnv *env, jclass c, jfloat x, jfloat y, jfloat z)
{
	NNN::Platform::Android::JNI_OnGyrometer(env, c, x, y, z);
}


/*==============================================================
 * 地磁仪（磁强计、磁场感应器）
 * Java_com_nnn_lib_nnnLib_OnMagnetometer()
 *==============================================================*/
NNN_ANDROID_JNI_FUNC(OnMagnetometer) (JNIEnv *env, jclass c, jfloat x, jfloat y, jfloat z)
{
	NNN::Platform::Android::JNI_OnMagnetometer(env, c, x, y, z);
}

#endif	// NNN_PLATFORM_ANDROID
