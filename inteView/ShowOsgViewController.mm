//
//  ShowOsgViewController.m
//  OSGIPhone
//
//  Created by tysoft on 14-9-23.
//
//

#import "ShowOsgViewController.h"
#include "OsgConverter.h"
#import "osgView.h"
#define isPad (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)

@interface ShowOsgViewController ()<UIGestureRecognizerDelegate,UITableViewDataSource,UITableViewDelegate,menuClassDelegate>
{
    UIView *backView;
    
    NSTimer *myTimer;
    
    NSTimer *anotherTimer;
    
    __block BOOL menuIsShow;
    
    __block BOOL menuClassIsShow;
    
    NSMutableDictionary *menuDetailDic;
    
    NSMutableArray *menuClassArr;//多级菜单
    
    osgView *bgView;//中间视图
    
    osgView *leftView;//左视图
    
    osgView *rightView;//右视图
    
    UITableView *menuTableView;
    
    CGFloat height;
    
    CGFloat screenHeight;
    
    CGFloat screenWidth;
    
    int step;//加载第几步
}

@end

@implementation ShowOsgViewController
@synthesize filePath;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {

        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    height = self.navigationController.navigationBar.frame.size.height + [[UIApplication sharedApplication] statusBarFrame].size.height;
    
    screenHeight = [UIScreen mainScreen].bounds.size.height;
    
    screenWidth = [UIScreen mainScreen].bounds.size.width;
    
    [self initTable];
    
    [self initOsgWindow:0];
    
    [self.view addSubview:menuTableView];
    [self.view addSubview:backView];
    
    // Do any additional setup after loading the view.
}

- (void)initTable{
    
    menuClassArr = [[NSMutableArray alloc]init];
    
    menuTableView = [[UITableView alloc]initWithFrame:CGRectMake(-80, (screenHeight-height)*1/10+height, 80, (screenHeight-height)*4/5)];
    menuTableView.delegate = self;
    menuTableView.dataSource = self;
    menuTableView.backgroundColor = [UIColor whiteColor];
    menuTableView.alpha = 0.5;
    
    [self addMenuBt];
}

- (void)initOsgWindow:(int)sender{
    
    [menuTableView removeFromSuperview];
    [backView removeFromSuperview];
    
    bgView = [[osgView alloc]initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height)];
    bgView.backgroundColor = [UIColor whiteColor];
    bgView.filePath = filePath;
    bgView.step = sender;
    bgView.delegate = self;
    [bgView initOsgWindow];
    
    [self.view addSubview:bgView];
    [self.view addSubview:menuTableView];
    [self.view addSubview:backView];
    
}

- (void)setMenuClass{
    
//    [self initTable];
    
    NSString *sender;
    std::string temStr;
    
    temStr = OsgConverter::getMenuClass();
    
    //这里注意转码问题，中文字符用下面coding方式，不然会乱码
    NSStringEncoding enc = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
    
    sender = [NSString stringWithCString:temStr.c_str() encoding:enc];
    
    //从osgconvert传值过来
    menuClassArr = [[NSMutableArray alloc]initWithArray:[sender componentsSeparatedByString:@","]];
    
    [menuTableView reloadData];
    
}

- (void)viewWillAppear:(BOOL)animated{
    UITapGestureRecognizer* singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTap:)];
    [self.view addGestureRecognizer:singleTap];
    //这个可以加到任何控件上,比如你只想响应WebView，我正好填满整个屏幕
    singleTap.delegate = self;
    singleTap.cancelsTouchesInView = NO;
}

- (void)viewWillDisappear:(BOOL)animated
{
//    [bgView.displayLink invalidate];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    
    return YES;
    
}

-(void)handleSingleTap:(UITapGestureRecognizer *)sender
{
    
    CGPoint point = [sender locationInView:self.view];
    
    NSLog(@"handleSingleTap!pointx:%f,y:%f",point.x,point.y);
    
    if (point.x > self.view.frame.size.width*2/3&&!menuIsShow) {
        
        [UIView animateWithDuration:0.5f animations:^{
            backView.frame = CGRectMake(self.view.frame.size.width - 55, self.view.frame.size.height/2-140, 80, 45*7);
        } completion:^(BOOL finished) {
            
            menuIsShow = YES;
            
            myTimer = [NSTimer scheduledTimerWithTimeInterval:2.0f target:self selector:@selector(hideMenu) userInfo:nil repeats:NO];
            
        }];
    }
    else if (point.x < self.view.frame.size.width*1/3&&!menuClassIsShow){
        
        [UIView animateWithDuration:0.5f animations:^{
            menuTableView.frame = CGRectMake(10, (screenHeight-height)*1/10+height, 80, (screenHeight-height)*4/5);
        } completion:^(BOOL finished) {
            
            menuClassIsShow = YES;
            
            anotherTimer = [NSTimer scheduledTimerWithTimeInterval:2.0f target:self selector:@selector(hideClassMenu) userInfo:nil repeats:NO];
            
        }];
    }
}

- (void)hideClassMenu{
    
    [UIView animateWithDuration:0.5f animations:^{
        
        menuClassIsShow = NO;
        
        menuTableView.frame = CGRectMake(-80, (screenHeight-height)*1/10+height, 80, (screenHeight-height)*4/5);
        
    }];
    
}

- (void)hideMenu{
    
    [UIView animateWithDuration:0.5f animations:^{
        
        menuIsShow = NO;
        
        backView.frame = CGRectMake(self.view.frame.size.width, self.view.frame.size.height/2-140, 80, 45*7);
        
    }];
}

- (void)addMenuBt {
    
    backView = [[UIView alloc]initWithFrame:CGRectMake(self.view.frame.size.width, self.view.frame.size.height/2-140, 80, 45*7)];
    backView.backgroundColor = [UIColor clearColor];
    
    NSArray *menuArr = nil;
    
    if (isPad) {
        menuArr = [[NSArray alloc]initWithObjects:@"轴侧图", @"前视图", @"后视图", @"左视图",@"右视图", @"俯视图", @"仰视图",nil];
    }else {
        menuArr = [[NSArray alloc]initWithObjects:@"轴", @"前", @"后", @"左",@"右", @"俯", @"仰",nil];
    }
    
    for (int i = 0; i < menuArr.count; i ++) {
        
        UIButton *menuBt = [[UIButton alloc]initWithFrame:CGRectMake(0,i*45, 80, 40)];
        
        if (!isPad) {
            
            menuBt.frame = CGRectMake(0, (i*45), 45, 40);
            
        }
        
        menuBt.backgroundColor = [UIColor colorWithRed:98/255.0 green:197/255.0 blue:250/255.0 alpha:0.5];
        [menuBt setTitle:[menuArr objectAtIndex:i] forState:UIControlStateNormal];
        [menuBt addTarget:self action:@selector(alertViewType:) forControlEvents:UIControlEventTouchUpInside];
        menuBt.tag = i+1;
        [backView addSubview:menuBt];

    }

}

#pragma mark -uitableveiwdatasource
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return menuClassArr.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    return 45;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"menu";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc]initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
        
        cell.selectionStyle = UITableViewCellSelectionStyleDefault;
//        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;//箭头
    }
    
    for (UIView *V in cell.contentView.subviews) {
        [V removeFromSuperview];
    }
    
    UILabel *menuLabel = [[UILabel alloc]initWithFrame:CGRectMake(0, 0, 80, 45)];
    menuLabel.text = [menuClassArr objectAtIndex:indexPath.row];
    menuLabel.font = [UIFont systemFontOfSize:12];
    menuLabel.textColor = [UIColor lightGrayColor];
    [cell.contentView addSubview:menuLabel];
    
    return cell;
}


#pragma mark -uitableviewdelegate
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
   
    NSIndexPath *selected = [tableView indexPathForSelectedRow];
    if(selected)
    {
        [tableView deselectRowAtIndexPath:indexPath animated:YES];
    }
    
    step = (int)indexPath.row;
    
    [self hideClassMenu];
    
    [anotherTimer invalidate];
    
    _displayLink = nil;
    windata = NULL;
    clearOsgView();
    
    [self initOsgWindow:step];
    
}

- (void)alertViewType:(UIButton *)bt {
    
    NavigationButtonDown((int)bt.tag);
    
    [myTimer invalidate];
    
    myTimer = [NSTimer scheduledTimerWithTimeInterval:2.0f target:self selector:@selector(hideMenu) userInfo:nil repeats:NO];
    
}

- (void)clicktest {
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)viewDidDisappear:(BOOL)animated {

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    _displayLink = nil;
//    windata = NULL;
    clearOsgView();
//    [super dealloc];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations.
    
    return YES;
    
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
