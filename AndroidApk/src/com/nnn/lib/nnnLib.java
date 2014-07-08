package com.nnn.lib;

import android.content.res.AssetManager;
import android.widget.EditText;

// Wrapper for native library
public class nnnLib
{
	public static native void		OnInit();
	public static native void		OnFinal();
	public static native void		OnRender();
	public static native void		OnSizeChanged(int w, int h);
	public static native void		OnPaused();
	public static native void		OnResume();

	public static native void		OnReciveSdcardPath(String path);
	public static native void		OnInitAssetManager(AssetManager mgr);

	public static native void		InitIME(nnnActivity activity, EditText textbox);
	public static native void		FinalIME();

	public static native void		OnTouches(	c_TouchPoint[] touches, int touches_count,
												int Msg, int point_x, int point_y, int touch_id );

	public static native void		OnRotation(int rotation);
	public static native void		OnAccelerometer(float x, float y, float z);
	public static native void		OnGyrometer(float x, float y, float z);
	public static native void		OnMagnetometer(float x, float y, float z);
}
