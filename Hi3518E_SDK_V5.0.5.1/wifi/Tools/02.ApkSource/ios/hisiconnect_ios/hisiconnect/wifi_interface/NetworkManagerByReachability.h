//
//  NetworkManagerByReachability.h
//  hisiconnect
//
//  Created by hi1311 on 2017/4/24.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#define GLobalNetworkManagerByReachability [NetworkManagerByReachability sharedInstance]

static  int SECURITY_NONE = 0;
static  int SECURITY_WEP = 1;
static  int SECURITY_PSK = 2;
static  int SECURITY_EAP = 3;
static  int SECURITY_ERR = 4;

@interface NetworkManagerByReachability : NSObject

+ (instancetype) sharedInstance;
- (BOOL)         isWiFiEnabled;
- (NSString*)    getWifiMACAddress;
- (NSString*)    getWifiSSID;
- (NSString *)   getLocalIPAddress:(NSString*) networkName;

@end



