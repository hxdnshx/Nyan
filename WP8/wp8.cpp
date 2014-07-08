#include "../../../../Src/common/common-macro.h"

#if (NNN_PLATFORM == NNN_PLATFORM_WP8)

#include "../../../../Src/nnnWP8Entry/nnnWP8Entry.h"

using namespace Windows::ApplicationModel::Core;

extern bool g_Enable_Accelerometer;	// 加速计（加速度计）
extern bool g_Enable_Orientation;	// 方向传感器（方向感应器）
extern bool g_Enable_Gyrometer;		// 陀螺测试仪（三轴螺旋仪）
extern bool g_Enable_Inclinometer;	// 测斜仪
extern bool g_Enable_Compass;		// 指南针（罗盘）

/*==============================================================
 * 入口点
 * main()
 *==============================================================*/
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource(	g_Enable_Accelerometer,	60,
																		g_Enable_Orientation,	60,
																		g_Enable_Gyrometer,		60,
																		g_Enable_Inclinometer,	60,
																		g_Enable_Compass,		60 );
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

#endif	// NNN_PLATFORM_WP8
