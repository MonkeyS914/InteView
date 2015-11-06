//
//  osgView.h
//  inteView
//
//  Created by Sunc on 15/11/2.
//  Copyright (c) 2015年 whtysf. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "osgNativeLib.h"
#include <osgViewer/api/IOS/GraphicsWindowIOS>

@protocol menuClassDelegate <NSObject>

- (void)setMenuClass;

@end

@interface osgView : UIView
{
//    CADisplayLink* _displayLink;
    osg::ref_ptr<osg::Referenced> windata;
}

- (void) initOsgWindow;

- (void) releaseObject;

@property (nonatomic, assign) int step;
@property (nonatomic, retain) NSString *filePath;
@property (nonatomic, retain) CADisplayLink* displayLink;
@property (nonatomic, assign) id <menuClassDelegate> delegate;

@end
