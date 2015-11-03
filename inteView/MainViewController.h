//
//  MainViewController.h
//  custom_view_test
//
//  Created by Stephan Huber on 04.05.11.
//  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
//

#import <UIKit/UIKit.h>
@interface MainViewController : UIViewController <UITableViewDataSource,UITableViewDelegate> {
    UITableView *fileTb;
    NSMutableArray *filePathArr;
    NSMutableArray *fileName;
    NSMutableArray *fileList;
}

@property (nonatomic ,retain) NSString *file_path;
@end
