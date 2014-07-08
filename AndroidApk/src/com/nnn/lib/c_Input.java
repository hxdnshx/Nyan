package com.nnn.lib;

// 触点
class c_TouchPoint
{
	public int	m_x;
	public int	m_y;
	public int	m_id;
};


// 触摸消息类型
class c_MessageType
{
	// m_wParam = <产生事件的触点索引>，m_lParam = <当前的触点数。如果是 Up 事件，则触点数再 +1>
	final static int Down			= 0;	// 触点按下事件（产生事件前的触点数 = 1 时触发，如 > 1，则触发 MULTI_TOUCH_DOWN）
	final static int Up				= 1;	// 触点松开事件（产生事件前的触点数 = 1 时触发，如 > 1，则触发 MULTI_TOUCH_UP）
	final static int Move			= 2;	// 触点移动事件（注意：产生该事件的触点索引永远是 0）
	final static int Cancel			= 3;	// 触点取消事件
	//final static int Outside		= 4;
	final static int PointerDown	= 5;	// 触点按下事件（产生事件前的触点数 > 1 时触发，如 = 1，则触发 TOUCH_DOWN）
	final static int PointerUp		= 6;	// 触点松开事件（产生事件前的触点数 > 1 时触发，如 = 1，则触发 TOUCH_UP）
};


// 设备的旋转方向
class c_RotationType
{
	final static int Unspecified	= 0;	// 未指定
	final static int Identity		= 1;	// 初始旋转方向
	final static int Rotate90		= 2;	// 逆时针转 90 度
	final static int Rotate180		= 3;	// 旋转 180 度
	final static int Rotate270		= 4;	// 逆时针转 270 度
};
