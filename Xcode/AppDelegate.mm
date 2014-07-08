//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//--------------------------------------------------------------------------------------

#import "AppDelegate.h"

#import "ViewController.h"

#include "../../../../Src/nnnEngine/nnn.h"

extern bool g_Enable_Compass;	// 指南针

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	//=====Modified by Star90x(2013.4.7)======(移除xib文件依赖)(Start)
	/*
	// Override point for customization after application launch.
	if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
		self.viewController = [[ViewController alloc] initWithNibName:@"ViewController_iPhone" bundle:nil];
	} else {
		self.viewController = [[ViewController alloc] initWithNibName:@"ViewController_iPad" bundle:nil];
	}
	*/
	self.viewController = [[ViewController alloc] init];
	//=====Modified by Star90x(2013.4.7)======(移除xib文件依赖)(End)
	self.viewController.preferredFramesPerSecond = 60;
	self.window.rootViewController = self.viewController;
	[self.window makeKeyAndVisible];

	//=====Added by Star90x(2013.2.21)=====(指南针)(Start)
	if(g_Enable_Compass)
	{
		// 判断指南针设备是否可用
		BOOL bHeadingAvailable = [CLLocationManager headingAvailable];
		if(bHeadingAvailable)
		{
			self.appLocationManager = [[CLLocationManager alloc] init];
			self.appLocationManager.delegate = self;
			[self.appLocationManager startUpdatingHeading];
		}
		else
			g_Enable_Compass = false;
	}
	//=====Added by Star90x(2013.2.21)=====(指南针)(End)

	return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.

	// 暂停处理
	NNN::Platform::IOS::OnPaused();

	// 暂停指南针更新
	if(g_Enable_Compass)
		[self.appLocationManager stopUpdatingHeading];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.

	// 恢复处理
	NNN::Platform::IOS::OnResume();

	// 恢复指南针更新
	if(g_Enable_Compass)
		[self.appLocationManager startUpdatingHeading];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

//=====Added by Star90x(2013.2.21)=====(指南针)(Start)
- (void)locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading
{
	NNN::Platform::IOS::OnCompass(newHeading.magneticHeading, newHeading.trueHeading);
}
//=====Added by Star90x(2013.2.21)=====(指南针)(End)

@end
