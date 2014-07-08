package com.nnn.lib;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.opengl.GLSurfaceView;

public class nnnRenderer implements GLSurfaceView.Renderer
{
	private int		m_width		= 0;
	private int		m_height	= 0;

	public nnnView	m_View;

	@Override
	public void onDrawFrame(GL10 gl)
	{
		nnnLib.OnRender();
		m_View.requestRender();
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		if(width != m_width || height != m_height)
		{
			nnnLib.OnSizeChanged(width, height);

			m_width = width;
			m_height = height;
		}
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		nnnLib.OnInit();
	}
}
