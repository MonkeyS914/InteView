//
//  osgView.m
//  inteView
//
//  Created by Sunc on 15/11/2.
//  Copyright (c) 2015年 whtysf. All rights reserved.
//

#import "osgView.h"
#include "OsgConverter.h"

@implementation osgView

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

- (void)initOsgWindow{
    
    NSString *exestr = [_filePath pathExtension];
    
    if([exestr isEqualToString:@"i3d"])
    {
        
    }
    else
    {
        _step = -1;
    }
    
    loadObject(_filePath);
    
    if (windata == NULL) {
        windata = new osgViewer::GraphicsWindowIOS::WindowData(self, osgViewer::GraphicsWindowIOS::WindowData::IGNORE_ORIENTATION);
    }
    
    initOsgNav(self.frame.size.width, self.frame.size.height, windata);
    
    osgDraw( _step, _isFromPic );
    
    //step = -1 时表示非i3d文件
    //step = 0 总装图
    //step = 1 步骤1
    //step = n 步骤n
    
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateScene:)];
    [_displayLink setFrameInterval:1];
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    
    if ([_delegate respondsToSelector:@selector(setMenuClass)]) {
        [_delegate setMenuClass];
    }

}

- (void)updateScene: (CADisplayLink *)sender {
    
    osgDraw( _step, _isFromPic );
    
}

- (void)releaseObject{
    
    [_displayLink removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    _displayLink = nil;
    windata = NULL;
//    clearContents();
    clearOsgView();
}

- (void)dealloc {

//    [_displayLink removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    _displayLink = nil;
    windata = NULL;
    clearOsgView();
    [super dealloc];
}

@end
