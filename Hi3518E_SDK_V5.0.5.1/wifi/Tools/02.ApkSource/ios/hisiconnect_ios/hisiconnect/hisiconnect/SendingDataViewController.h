//
//  SendingDataViewController.h
//  hisiconnect
//
//  Created by hi1311 on 2017/4/24.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#ifndef SendingDataViewController_h
#define SendingDataViewController_h

#import <UIKit/UIKit.h>
#import "WifiNetworkInfo.h"
#import "GCDAsyncSocket.h"

static const uint16_t hostPort = 0x3516;
#define               WELCOME_MSG  0
#define               READ_TIMEOUT 40.0

@interface SendingDataViewController : UIViewController <UITextFieldDelegate>

@property (nonatomic, strong) NSString         *connectModel;
@property (nonatomic, strong) WifiNetworkInfo  *connectAPInfo;

-(IBAction)toggleButton:(UIButton *) sendDataButton;

@property (nonatomic) NSMutableArray    *notifications;         // 通知队列
@property (nonatomic) NSThread          *notificationThread;    // 期望线程
@property (nonatomic) NSLock            *notificationLock;      // 用于对通知队列加锁的锁对象，避免线程冲突
@property (nonatomic) NSMachPort        *notificationPort;      // 用于向期望线程发送信号的通信端口

@property (strong, nonatomic)GCDAsyncSocket * serverSocket;
@property (strong, nonatomic)GCDAsyncSocket * clientSocket;

@end


#endif /* SendingDataViewController_h */
