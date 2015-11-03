//
//  AppDelegate.h
//  inteView
//
//  Created by tysoft on 14-10-20.
//  Copyright (c) 2014年 whtysf. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MainViewController.h"



@interface AppDelegate : UIResponder <UIApplicationDelegate> {
    MainViewController *mainViewController;
}

@property (strong, nonatomic) UIWindow *window;
@property (nonatomic, retain) MainViewController *mainViewController;

@end

