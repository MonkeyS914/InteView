//
//  ShowOsgViewController.h
//  OSGIPhone
//
//  Created by tysoft on 14-9-23.
//
//

#import <UIKit/UIKit.h>
#import "osgNativeLib.h"
#include <osgViewer/api/IOS/GraphicsWindowIOS>

@interface ShowOsgViewController : UIViewController{
    NSString *filePath;
    CADisplayLink* _displayLink;
    osg::ref_ptr<osg::Referenced> windata;
//    UIView *bgView;
}
@property(nonatomic, retain)NSString *filePath;

@end
