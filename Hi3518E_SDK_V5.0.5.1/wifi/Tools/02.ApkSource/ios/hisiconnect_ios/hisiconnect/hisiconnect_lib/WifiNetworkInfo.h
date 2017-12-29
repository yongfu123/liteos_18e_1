//
//  WifiNetworkInfo.h
//  hisiconnect
//
//  Created by hi1311 on 2017/4/28.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#ifndef WifiNetworkInfo_h
#define WifiNetworkInfo_h



@interface WifiNetworkInfo : NSObject
@property (nonatomic, assign) NSInteger port;
@property (nonatomic, assign) NSInteger security;
@property (nonatomic, strong) NSString *ssid;
@property (nonatomic, strong) NSString *password;
@property (nonatomic, strong) NSString *deviceName;
@property (nonatomic, strong) NSString *ip;
@property (nonatomic, assign) NSInteger onlineProto;
@property (nonatomic, strong) NSString *bssid;
@end



#endif /* WifiNetworkInfo_h */
