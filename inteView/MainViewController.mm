//
//  MainViewController.m
//  custom_view_test
//
//  Created by Stephan Huber on 04.05.11.
//  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
//

#import "MainViewController.h"
#import "ShowOsgViewController.h"


@implementation MainViewController
{
    NSString *documentPath;
}
@synthesize file_path;

//-(void)viewWillDisappear:(BOOL)animated {
//    [[self navigationController]setNavigationBarHidden:YES animated:YES];
//}
//
//-(void)viewWillAppear:(BOOL)animated {
//    [[self navigationController]setNavigationBarHidden:NO animated:YES];
//}

- (void)viewDidLoad {
	[super viewDidLoad];
    fileTb = [[UITableView alloc]initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height)];
    fileTb.backgroundColor = [UIColor whiteColor];
    fileTb.dataSource = self;
    fileTb.delegate = self;
    [self.view addSubview:fileTb];
    [fileTb release];
    if (!file_path) {
        file_path = @"";
    }
    filePathArr = [[NSMutableArray alloc]init];
    fileName = [[NSMutableArray alloc]init];
    fileList = [[NSMutableArray alloc]init];
    [self readFile];
}

-(void)readFile {
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *documentDir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSError *error = nil;
    documentPath = [documentDir stringByAppendingPathComponent:file_path];
    [fileList addObjectsFromArray:[fileManager contentsOfDirectoryAtPath:documentPath error:&error]];
    BOOL isDir = NO;
    
    //在上面那段程序中获得的fileList中列出文件夹名
    for (NSString *file in fileList) {
        NSString *path = [documentPath stringByAppendingPathComponent:file];
        [fileManager fileExistsAtPath:path isDirectory:(&isDir)];
        if (!isDir) {
            NSString *exestr = [file pathExtension];
            NSString *fileNameStr = [file stringByDeletingPathExtension];
            if (![exestr isEqualToString:@"plist"] && ![fileNameStr isEqualToString:@"SAVR_FILE"]) {
                [fileName addObject:file];
                [filePathArr addObject:path];
            }
        }else {
            
        }
    }
    
    [fileTb reloadData];
}


- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return 1;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return fileName.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44;
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *SimpleTableIdentifier = @"SimpleTableIdentifier";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:
                             SimpleTableIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                      reuseIdentifier: SimpleTableIdentifier];
        cell.selectionStyle = UITableViewCellSelectionStyleBlue;
        cell.backgroundColor = [UIColor whiteColor];
        cell.backgroundView = nil;
        
        
    }
    for (UIView *v in cell.contentView.subviews) {
        [v removeFromSuperview];
    }
    
    
    UILabel *fileGroupNameLb = [[UILabel alloc]initWithFrame:CGRectMake(15, 0, self.view.frame.size.width - 88, 44)];
    fileGroupNameLb.text = [NSString stringWithFormat:@"%@",[fileName objectAtIndex:indexPath.row]];
    fileGroupNameLb.textColor = [UIColor blackColor];
    fileGroupNameLb.backgroundColor = [UIColor clearColor];
    fileGroupNameLb.font = [UIFont systemFontOfSize:14];
    [cell.contentView addSubview:fileGroupNameLb];
    [fileGroupNameLb release];
    
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    NSString *filenName = [filePathArr objectAtIndex:indexPath.row];
    NSString *exeStr = [filenName pathExtension];
    
    if ([exeStr isEqual:@"i3d"]) {
        
    }
    
    ShowOsgViewController *showOsgVc = [[ShowOsgViewController alloc]init];
    showOsgVc.filePath = [NSString stringWithFormat:@"%@",[filePathArr objectAtIndex:indexPath.row]];
    [self.navigationController pushViewController:showOsgVc animated:YES];
    [showOsgVc release];
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc. that aren't in use.
}


- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}



// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	// Return YES for supported orientations.
	return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}



- (void)dealloc {
    [super dealloc];
}


@end
