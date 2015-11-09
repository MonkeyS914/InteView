//
//  osgNativeLib.h
//  OSGIPhone
//
//  Created by tysoft on 14-9-15.
//
//

#import <Foundation/Foundation.h>
#include <osgViewer/api/IOS/GraphicsWindowIOS>
@interface osgNativeLib : NSObject

void loadObject(NSString *address);

void initOsgSimNav();
void initOsgNav(int width, int height, osg::ref_ptr<osg::Referenced>& graphicsContext);

void osgDraw( int step, int isFromPic );

void clearContents();

void mouseButtonPressEvent(float x,float y, int button);

void mouseButtonReleaseEvent(float x,float y, int button);

void mouseMoveEvent(float x,float y);

void keyboardDown(int key);

void keyboardUp(int key);

void setClearColor(int red,int green, int blue);

//int[] getClearColor();

void loadObject(NSString *address, NSString *name);

void unLoadObject(int number);

NSString*getObjectNames();

void singletouchBeganEvent(int idnum,int phase,float x,float y);

void singletouchMovedEvent(int idnum,int phase,float x,float y);

void singletouchEndedEvent(int idnum,int phase,float x,float y);

void doubleTouchEvent(int phase,int id0,float x0,float y0,int id1,float x1,float y1);

void NavigationButtonDown(int viewtype);

void clearOsgView();

@end
