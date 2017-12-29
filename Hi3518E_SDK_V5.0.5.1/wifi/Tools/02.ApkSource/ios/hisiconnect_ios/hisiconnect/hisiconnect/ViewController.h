//
//  ViewController.h
//  hisiconnect
//
//  Created by hi1311 on 2017/4/22.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Reachability.h"

@interface ViewController : UIViewController

@property (nonatomic, strong) IBOutlet UITableView *tableView;
@property (nonatomic) Reachability *hostReachability;

@end

