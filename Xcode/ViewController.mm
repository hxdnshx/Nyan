//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//--------------------------------------------------------------------------------------

#import <CoreMotion/CoreMotion.h>
#import "ViewController.h"

#include "../../../../Src/nnnEngine/nnn.h"

extern bool g_Enable_Accelerometer;
extern bool g_Enable_Gyrometer;

@interface ViewController () {
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) CMMotionManager *appMotionManager;

- (void)setupGL;
- (void)tearDownGL;
@end

@implementation ViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

	if (!self.context) {
		NSLog(@"Failed to create ES context");
	}

	GLKView *view = (GLKView *)self.view;
	view.context = self.context;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
	view.multipleTouchEnabled = YES;

	[self setupGL];

	NNN::Platform::IOS::IME_Init((__bridge void*)self);	// 初始化 iOS 输入法

	//=====Added by Star90x(2013.2.22)=====(加速度计/陀螺仪初始化)(Start)
	if(g_Enable_Accelerometer || g_Enable_Gyrometer)
	{
		self.appMotionManager = [[CMMotionManager alloc] init];

		// 加速度计
		if(g_Enable_Accelerometer)
		{
			if(self.appMotionManager.accelerometerAvailable)
			{
				// 更新频率60Hz
				self.appMotionManager.accelerometerUpdateInterval = 1.0f / 60.0f;
				[self.appMotionManager startAccelerometerUpdates];
			}
			else
				g_Enable_Accelerometer = false;
		}

		// 陀螺仪
		if(g_Enable_Gyrometer)
		{
			if(self.appMotionManager.gyroAvailable)
			{
				// 更新频率60Hz
				self.appMotionManager.gyroUpdateInterval = 1.0f / 60.0f;
				[self.appMotionManager startGyroUpdates];
			}
			else
				g_Enable_Gyrometer = false;
		}
	}
	//=====Added by Star90x(2013.2.22)=====(加速度计/陀螺仪初始化)(End)
}

- (void)dealloc
{
	NNN::Platform::IOS::OnFinal();	//=====Added by FPE(2012.4.4)=====(清理)

	//=====Added by Star90x(2013.2.22)=====(加速度计/陀螺仪停止更新)(Start)
	// 加速度计
	if(g_Enable_Accelerometer)
		[self.appMotionManager stopAccelerometerUpdates];

	// 陀螺仪
	if(g_Enable_Gyrometer)
		[self.appMotionManager stopGyroUpdates];
	//=====Added by Star90x(2013.2.22)=====(加速度计/陀螺仪停止更新)(End)

	[self tearDownGL];

	if([EAGLContext currentContext] == self.context)
		[EAGLContext setCurrentContext:nil];
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];

	if ([self isViewLoaded] && ([[self view] window] == nil)) {
		self.view = nil;

		[self tearDownGL];

		if ([EAGLContext currentContext] == self.context) {
			[EAGLContext setCurrentContext:nil];
		}
		self.context = nil;
	}

	// Dispose of any resources that can be recreated.
}

//=====Added By Star90x(2012.10.9)=====(屏幕旋转处理)(Start)
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	switch(toInterfaceOrientation)
	{
	case UIDeviceOrientationPortrait:
	case UIDeviceOrientationPortraitUpsideDown:
	case UIDeviceOrientationLandscapeLeft:
	case UIDeviceOrientationLandscapeRight:
		return TRUE;
	}

	return FALSE;
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
	bool swap_width_height = false;
	NNN::Input::Rotation::es_Rotation rotation = NNN::Input::Rotation::es_Rotation::Unspecified;

	switch(toInterfaceOrientation)
	{
	case UIDeviceOrientationLandscapeLeft:
		rotation = NNN::Input::Rotation::es_Rotation::Rotate90;
		swap_width_height = true;
		break;

	case UIDeviceOrientationLandscapeRight:
		rotation = NNN::Input::Rotation::es_Rotation::Rotate270;
		swap_width_height = true;
		break;

	case UIDeviceOrientationPortrait:
		rotation = NNN::Input::Rotation::es_Rotation::Identity;
		break;

	case UIDeviceOrientationPortraitUpsideDown:
		rotation = NNN::Input::Rotation::es_Rotation::Rotate180;
		break;

	default:
		return;
	}

	int width	= (int)(self.view.contentScaleFactor * [[UIScreen mainScreen] bounds].size.width);
	int height	= (int)(self.view.contentScaleFactor * [[UIScreen mainScreen] bounds].size.height);
	
	if(swap_width_height)
		std::swap(width, height);

	NNN::Platform::IOS::OnRotation(rotation);
	NNN::Platform::IOS::OnSizeChanged(width, height);
}
//=====Added By Star90x(2012.10.9)=====(屏幕旋转处理)(End)

- (void)setupGL
{
	[EAGLContext setCurrentContext:self.context];

	//=====Added by FPE(2012.4.4)=====(初始化 & 记录客户区大小)(Start)
	int width	= (int)(self.view.contentScaleFactor * [[UIScreen mainScreen] bounds].size.width);
	int height	= (int)(self.view.contentScaleFactor * [[UIScreen mainScreen] bounds].size.height);

	//=====Added by Star90x(2014.2.12)=====(初始化旋转方向)(Start)
	switch(self.interfaceOrientation)
	{
	case UIInterfaceOrientationPortrait:
		NNN::Input::Rotation::SetRotation(NNN::Input::Rotation::es_Rotation::Identity);
		break;

	case UIInterfaceOrientationPortraitUpsideDown:
		NNN::Input::Rotation::SetRotation(NNN::Input::Rotation::es_Rotation::Rotate180);
		break;

	case UIInterfaceOrientationLandscapeLeft:
		NNN::Input::Rotation::SetRotation(NNN::Input::Rotation::es_Rotation::Rotate90);
		std::swap(width, height);
		break;

	case UIInterfaceOrientationLandscapeRight:
		NNN::Input::Rotation::SetRotation(NNN::Input::Rotation::es_Rotation::Rotate270);
		std::swap(width, height);
		break;
	}
	//=====Added by Star90x(2014.2.12)=====(初始化旋转方向)(End)
	NNN::Platform::IOS::OnSizeChanged(width, height);	// 仅记录客户区大小（在 Init 内部会重新设置一次 size）

	// 由于此时旋转器并未初始化，暂不修改设备朝向
	//NNN::Platform::IOS::OnRotation(...);

	NNN::Platform::IOS::OnInit((__bridge void*)self.view, self.view.contentScaleFactor);
	//=====Added by FPE(2012.4.4)=====(初始化 & 记录客户区大小)(End)
}

- (void)tearDownGL
{
	[EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
	//=====Added by Star90x(2013.2.22)=====(传感器状态更新)(Start)
	// 加速度计
	if(g_Enable_Accelerometer)
	{
		CMAccelerometerData *pAccData = self.appMotionManager.accelerometerData;
		NNN::Platform::IOS::OnAccelerometer(pAccData.acceleration.x,
											pAccData.acceleration.y,
											-pAccData.acceleration.z);	// 坐标系统一，z 轴取负
	}

	// 陀螺仪
	if(g_Enable_Gyrometer)
	{
		CMGyroData *pGyroData = self.appMotionManager.gyroData;
		NNN::Platform::IOS::OnGyrometer(pGyroData.rotationRate.x,
										pGyroData.rotationRate.y,
										pGyroData.rotationRate.z);
	}
	//=====Added by Star90x(2013.2.22)=====(传感器状态更新)(End)
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	NNN::Platform::IOS::OnRender();	//=====Added by FPE(2012.4.4)=====(渲染)
}

//====================Added by FPE(2012.4.4)====================(触摸处理)(Start)
/*==============================================================
 * 触摸处理
 * touchesBegan()
 * touchesMoved()
 * touchesEnded()
 * touchesCancelled()
 *==============================================================*/
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	NNN::Platform::IOS::OnPointerPressed((__bridge void*)event.allTouches, (__bridge void*)self.view);
}
//--------------------------------------------------
-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	NNN::Platform::IOS::OnPointerMoved((__bridge void*)event.allTouches, (__bridge void*)self.view);
}
//--------------------------------------------------
-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	NNN::Platform::IOS::OnPointerReleased((__bridge void*)event.allTouches, (__bridge void*)self.view);
}
//--------------------------------------------------
-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	NNN::Platform::IOS::OnPointerCancelled((__bridge void*)event.allTouches, (__bridge void*)self.view);
}
//====================Added by FPE(2012.4.4)====================(触摸处理)(End)

@end
