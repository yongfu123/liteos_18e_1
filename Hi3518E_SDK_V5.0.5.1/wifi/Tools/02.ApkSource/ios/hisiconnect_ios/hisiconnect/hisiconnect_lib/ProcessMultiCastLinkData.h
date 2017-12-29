//
//  ProcessMultiCastLinkData.h
//  hisiconnect
//
//  Created by hi1311 on 2017/5/5.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#ifndef ProcessMultiCastLinkData_h
#define ProcessMultiCastLinkData_h
#import "WifiNetworkInfo.h"
#define GLobalProcessMultiCastLinkData [ProcessMultiCastLinkData sharedInstance]

@interface ProcessMultiCastLinkData : NSObject
+ (instancetype) sharedInstance;
-(Boolean)constructMultiCastLinkMessageToSend:(WifiNetworkInfo *)wifiNetworkInfo;
-(Boolean)startSendMultiBroadcast:(Boolean)enAllowSendData:(unsigned int)port;
-(void)stopSendMultiBroadcast;

@end
#endif /* ProcessMultiCastLinkData_h */
