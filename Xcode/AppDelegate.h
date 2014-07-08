//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//--------------------------------------------------------------------------------------

#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@class ViewController;

@interface AppDelegate : UIResponder <UIApplicationDelegate, CLLocationManagerDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) ViewController *viewController;

@property (strong, nonatomic) CLLocationManager *appLocationManager;

@end
