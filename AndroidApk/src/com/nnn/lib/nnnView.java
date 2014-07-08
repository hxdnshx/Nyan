package com.nnn.lib;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;

public class nnnView extends GLSurfaceView
{
	private static nnnView		m_s_self;
	private static String		m_s_TAG		= "nnnView";

	private c_TouchPoint[]		m_Touches	= new c_TouchPoint[10];	// 触点数据

	public nnnView(Context context, int depth, int stencil)
	{
		super(context);

		for(int i=0; i<m_Touches.length; ++i)
			m_Touches[i] = new c_TouchPoint();

		init(depth, stencil);
	}

	protected void init(int depth, int stencil)
	{
		m_s_self = this;

		/* By default, GLSurfaceView() creates a RGB_565 opaque surface.
		 * If we want a translucent one, we should change the surface's
		 * format here, using PixelFormat.TRANSLUCENT for GL Surfaces
		 * is interpreted as any 32-bit surface with alpha by SurfaceFlinger.
		 */
		this.getHolder().setFormat(PixelFormat.TRANSLUCENT);

		/* Setup the context factory for 2.0 rendering. */
		setEGLContextFactory(new ContextFactory());

		/* We need to choose an EGLConfig that matches the format of
		 * our surface exactly. This is going to be done in our
		 * custom config chooser.
		 */
		setEGLConfigChooser(new ConfigChooser(8, 8, 8, 8, depth, stencil));

		setEGLContextClientVersion(2);

		/* Set the renderer responsible for frame rendering */
		nnnRenderer renderer = new nnnRenderer();
		renderer.m_View = this;
		setRenderer(renderer);

		setRenderMode(RENDERMODE_WHEN_DIRTY);
	}

	@Override
	public void onPause()
	{
		queueEvent(	new Runnable()
					{
						@Override
						public void run()
						{
							nnnLib.OnPaused();
						}
					}
		);
		super.onPause();
	}

	@Override
	public void onResume()
	{
		queueEvent(	new Runnable()
					{
						@Override
						public void run()
						{
							nnnLib.OnResume();
						}
					}
		);
		super.onResume();
	}

	public void onFinal()
	{
		queueEvent(	new Runnable()
					{
						@Override
						public void run()
						{
							nnnLib.OnFinal();
						}
					}
		);
	}

	@Override
	public boolean onTouchEvent(final MotionEvent event)
	{
		queueEvent(	new Runnable()
					{
						@Override
						public void run()
						{
							int touches_count = event.getPointerCount();

							// 事件
							int Msg = 0;
							int action = event.getAction();
							switch(action & MotionEvent.ACTION_MASK)
							{
							case MotionEvent.ACTION_DOWN:			Msg = c_MessageType.Down;		break;
							case MotionEvent.ACTION_UP:				Msg = c_MessageType.Up;			break;
							case MotionEvent.ACTION_MOVE:			Msg = c_MessageType.Move;		break;
							case MotionEvent.ACTION_CANCEL:			Msg = c_MessageType.Cancel;		break;
							//case MotionEvent.ACTION_OUTSIDE:		Msg = c_MessageType.Outside;	break;
							case MotionEvent.ACTION_POINTER_DOWN:	Msg = c_MessageType.PointerDown;break;
							case MotionEvent.ACTION_POINTER_UP:		Msg = c_MessageType.PointerUp;	break;
							}

							int touch_index	= (action & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;
							int point_x		= (int)event.getX(touch_index);
							int point_y		= (int)event.getY(touch_index);
							int touch_id	= event.getPointerId(touch_index);

							boolean is_up = (	Msg == c_MessageType.Up			||
												Msg == c_MessageType.PointerUp	||
												Msg == c_MessageType.Cancel );

							// 写入触点数据
							int idx = 0;
							for(int i=0; i<touches_count; ++i)
							{
								if(touch_index == i && is_up)
									continue;

								m_Touches[idx].m_x	= (int)event.getX(i);
								m_Touches[idx].m_y	= (int)event.getY(i);
								m_Touches[idx].m_id	= event.getPointerId(i);
								++idx;
							}

							if(is_up)
								--touches_count;

							nnnLib.OnTouches(m_Touches, touches_count, Msg, point_x, point_y, touch_id);
						}
					}
		);
		return true;
	}

	@Override
	public boolean onTrackballEvent(MotionEvent event)
	{
		return true;
	}

	//=======================================================================================

	private static class ContextFactory implements GLSurfaceView.EGLContextFactory
	{
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

		public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig)
		{
			Log.w(m_s_TAG, "creating OpenGL ES 2.0 context");
			checkEglError("Before eglCreateContext", egl);

			int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
			EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
			checkEglError("After eglCreateContext", egl);

			return context;
		}

		public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context)
		{
			egl.eglDestroyContext(display, context);
		}
	}

	private static void checkEglError(String prompt, EGL10 egl)
	{
		int error;
		while((error = egl.eglGetError()) != EGL10.EGL_SUCCESS)
			Log.e(m_s_TAG, String.format("%s: EGL error: 0x%x", prompt, error));
	}

	private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser
	{
		// Subclasses can adjust these values:
		protected	int		m_RedSize;
		protected	int		m_GreenSize;
		protected	int		m_BlueSize;
		protected	int		m_AlphaSize;
		protected	int		m_DepthSize;
		protected	int		m_StencilSize;
		private		int[]	m_Value			= new int[1];

		public ConfigChooser(int r, int g, int b, int a, int depth, int stencil)
		{
			m_RedSize		= r;
			m_GreenSize		= g;
			m_BlueSize		= b;
			m_AlphaSize		= a;
			m_DepthSize		= depth;
			m_StencilSize	= stencil;
		}

		/* This EGL config specification is used to specify 2.0 rendering.
		 * We use a minimum size of 4 bits for red/green/blue, but will
		 * perform actual matching in chooseConfig() below.
		 */
		private static int		k_EGL_OPENGL_ES2_BIT = 4;
		private static int[]	m_s_configAttribs2 =
		{
			EGL10.EGL_RED_SIZE,			4,
			EGL10.EGL_GREEN_SIZE,		4,
			EGL10.EGL_BLUE_SIZE,		4,
			EGL10.EGL_ALPHA_SIZE,		4,

			EGL10.EGL_LEVEL,			0,
			EGL10.EGL_SURFACE_TYPE,		EGL10.EGL_WINDOW_BIT,
			EGL10.EGL_RENDERABLE_TYPE,	k_EGL_OPENGL_ES2_BIT,
			EGL10.EGL_NATIVE_RENDERABLE,0,

			EGL10.EGL_DEPTH_SIZE,		EGL10.EGL_DONT_CARE,
			EGL10.EGL_STENCIL_SIZE,		EGL10.EGL_DONT_CARE,

			EGL10.EGL_NONE
		};

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
		{
			/* Get the number of minimally matching EGL configurations */
			int[] num_config = new int[1];
			egl.eglChooseConfig(display, m_s_configAttribs2, null, 0, num_config);

			int numConfigs = num_config[0];

			if(numConfigs <= 0)
				throw new IllegalArgumentException("No configs match configSpec");

			/* Allocate then read the array of minimally matching EGL configs */
			EGLConfig[] configs = new EGLConfig[numConfigs];
			egl.eglChooseConfig(display, m_s_configAttribs2, configs, numConfigs, num_config);

			/* Now return the "best" one */
			return chooseConfig(egl, display, configs);
		}

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs)
		{
			for(EGLConfig config : configs)
			{
				int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
				int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);

				// We need at least m_DepthSize and m_StencilSize bits
				if(d < m_DepthSize || s < m_StencilSize)
					continue;

				// We want an *exact* match for red/green/blue/alpha
				int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
				int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
				int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
				int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);

				if(r == m_RedSize && g == m_GreenSize && b == m_BlueSize && a == m_AlphaSize)
					return config;
			}
			return null;
		}

		private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue)
		{
			if(egl.eglGetConfigAttrib(display, config, attribute, m_Value))
				return m_Value[0];

			return defaultValue;
		}
	}
}
