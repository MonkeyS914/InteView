//
//  osgNativeLib.m
//  OSGIPhone
//
//  Created by tysoft on 14-9-15.
//
//

#import "osgNativeLib.h"
#include "OsgMainApp.hpp"
@implementation osgNativeLib

OsgMainApp mainApp;

void initOsgSimNav(){
    mainApp.initSimOsgWindow();
}

void initOsgNav(int width, int height, osg::ref_ptr<osg::Referenced>& windata){
    mainApp.initOsgWindow(0, 0, width, height, windata);
}

void osgDraw( int step,int isFromPic ){
    mainApp.draw( step, isFromPic );
}

void clearContents(){
    mainApp.clearScene();
}

void mouseButtonPressEvent(float x,float y, int button){
    mainApp.mouseButtonPressEvent(x/500, x/500, button);
}

void mouseButtonReleaseEvent(float x,float y, int button){
    mainApp.mouseButtonReleaseEvent(x/500,y/500,button);
}

void mouseMoveEvent(float x,float y){
    mainApp.mouseMoveEvent(x/500,y/500);
}

void keyboardDown(int key){
    mainApp.keyboardDown(key);
}

void keyboardUp(int key){
    mainApp.keyboardUp(key);
}

void setClearColor(int red,int green, int blue){
    osg::Vec4 tVec((float) red / 255.0f, (float) green / 255.0f, (float) blue / 255.0f, 0.0f);
    mainApp.setClearColor(tVec);
}

void loadObject(NSString *address){
    const char *nativeAddress = [address UTF8String];
    mainApp.loadObject(std::string(nativeAddress));
}

void loadObject(NSString *address, NSString *name){
    const char *nativeAddress = [address UTF8String];
    const char *nativeName = [name UTF8String];
    mainApp.loadObject(std::string(nativeName),std::string(nativeAddress));

}

void unLoadObject(int number){
    mainApp.unLoadObject(number);
}

void singletouchBeganEvent(int idnum,int phase,float x,float y){
    mainApp.touchBeganEvent(idnum, phase,x/500,y/500);
}

void singletouchMovedEvent(int idnum,int phase,float x,float y){
    mainApp.touchMovedEvent(idnum, phase,x/500,y/500);
}

void singletouchEndedEvent(int idnum,int phase,float x,float y){
    mainApp.touchEndedEvent(idnum, phase,x/500,y/500);
}

void doubleTouchEvent(int phase,int idzero,float xzero,float yzero,int idone,float xone,float yone){
    mainApp.doubleTouchEvent(phase,idzero,xzero/5,yzero/5,idone,xone/5,yone/5);
}

void NavigationButtonDown(int viewtype){
     mainApp.NavigationButtonDown(viewtype);
}

void clearOsgView() {
    mainApp.releaseView();
}

@end
