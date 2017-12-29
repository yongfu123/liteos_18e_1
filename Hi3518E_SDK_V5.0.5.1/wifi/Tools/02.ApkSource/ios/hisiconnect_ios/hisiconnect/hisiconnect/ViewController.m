//
//  ViewController.m
//  hisiconnect
//
//  Created by hi1311 on 2017/4/22.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#import <SystemConfiguration/SystemConfiguration.h>
#import <SystemConfiguration/CaptiveNetwork.h>
#import <MobileCoreServices/MobileCoreServices.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>

#include "NetworkManagerByReachability.h"
#import "ViewController.h"
#import "SendingDataViewController.h"
#import "WifiNetworkInfo.h"


@interface ViewController() <UITableViewDelegate,UITableViewDataSource>
{
    NSArray *recipes;
}
@end

@implementation ViewController

@synthesize tableView; // Add this line of code


- (void)viewDidLoad {
    [super viewDidLoad];
    
    BOOL isWIFI =[GLobalNetworkManagerByReachability isWiFiEnabled];
    if (!isWIFI) {//如果WiFi没有打开，作出弹窗提示
        [self alertController:@"Wi-Fi未打开，继续操作需要打开Wi-Fi，是否前往？"];
    }
    
    NSString *connectedSSID = [GLobalNetworkManagerByReachability getWifiSSID];
    NSString *connectedWifiIP = [GLobalNetworkManagerByReachability getLocalIPAddress:@"en0"];
    if(connectedSSID == nil || connectedWifiIP == nil)
    {
       [self alertController:@"Wi-Fi未关联，继续操作需要打开Wi-Fi进行关联，是否前往？"];
    }

    self.edgesForExtendedLayout = UIRectEdgeNone;
    recipes = [NSArray arrayWithObjects:@"AP模式", @"组播模式", nil];
    
}


- (void)applicationWillEnterForeground:(NSNotification *)notification
{
    //进入前台时调用此函数
    BOOL isWIFI =[GLobalNetworkManagerByReachability isWiFiEnabled];
    if (!isWIFI) {//如果WiFi没有打开，作出弹窗提示
        [self alertController:@"Wi-Fi未打开，继续操作需要打开Wi-Fi，是否前往？"];
    }
    NSString *connectedSSID = [GLobalNetworkManagerByReachability getWifiSSID];
    NSString *connectedWifiIP = [GLobalNetworkManagerByReachability getLocalIPAddress:@"en0"];
    if(connectedSSID == nil || connectedWifiIP == nil)
    {
        [self alertController:@"Wi-Fi未关联，继续操作需要打开Wi-Fi进行关联，是否前往？"];
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    UIApplication *app = [UIApplication sharedApplication];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationWillEnterForeground:)
                                                 name:UIApplicationWillEnterForegroundNotification
                                               object:app];
}

- (void)viewDidDisappear:(BOOL)animated 
{ 
    [[NSNotificationCenter defaultCenter] removeObserver:self]; 
}

- (void)alertController:(NSString *)alertMessage
{
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"警告" message:alertMessage preferredStyle:UIAlertControllerStyleAlert];
    
    [alert addAction:[UIAlertAction actionWithTitle:@"前往" style:UIAlertActionStyleDestructive handler:^(UIAlertAction *action) {
        NSURL *url = [NSURL URLWithString:@"Prefs:root=WIFI"];

        [[UIApplication sharedApplication] openURL:url];
        
        /*used in ios10*/
        //Class LSApplicationWorkspace = NSClassFromString(@"LSApplicationWorkspace");
        //[[LSApplicationWorkspace performSelector:@selector(defaultWorkspace)] performSelector:@selector(openSensitiveURL:withOptions:) withObject:url withObject:nil];
    }]];
    [alert addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        NSLog(@"点击了取消按钮");
        exit(0);
    }]];
    
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {

    return [recipes count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *simpleTableIdentifier = @"RecipeCell";
    
    [tableView setSectionIndexBackgroundColor:[UIColor clearColor]];
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
    
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                      reuseIdentifier:simpleTableIdentifier];
    }
    cell.textLabel.text = [recipes objectAtIndex:indexPath.row];
    UIImage *image = [UIImage imageNamed:@"linktype.png"];
    cell.imageView.image = image;
    
    
    NSLog(@"tableView,%@",cell.textLabel.text);
    return cell;
}
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selectionStyle = UITableViewCellAccessoryNone;
}

-(UITableViewCell *)tableView:cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    for(UIView *view in [tableView subviews])
    {
        if([view respondsToSelector:@selector(setSectionIndexColor:)])
        {
            //tableView.sectionIndexBackgroundColor = [UIColor clearColor];
            [view performSelector:@selector(setIndexPath:) withObject:[UIColor whiteColor]];
            //[view setBackgroundColor:[UIColor clearColor]];
        }
    }
    
    return tableView;
}



- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    NSString           *cnnectedAPInfo;
    WifiNetworkInfo    *networkInfo = [[WifiNetworkInfo alloc] init];
    networkInfo.ssid   = [GLobalNetworkManagerByReachability getWifiSSID];
    networkInfo.bssid   = [GLobalNetworkManagerByReachability getWifiMACAddress];
    networkInfo.ip      = [GLobalNetworkManagerByReachability getLocalIPAddress:@"en0"];
    NSLog(@"networkInfo.ip,%@",networkInfo.ip);
    
    if ([segue.identifier isEqualToString:@"shareDeviceWifiInfo"]) {
        NSIndexPath *indexPath = [self.tableView indexPathForSelectedRow];
        SendingDataViewController *destViewController = segue.destinationViewController;
        cnnectedAPInfo = [recipes objectAtIndex:indexPath.row];
        destViewController.connectModel = [recipes objectAtIndex:indexPath.row];
        destViewController.connectAPInfo = networkInfo;
    }
}


- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:kReachabilityChangedNotification object:nil];
}


-(void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

@end
