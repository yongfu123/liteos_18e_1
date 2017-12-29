//
//  TCPClientSocket.h
//  hisiconnect
//
//  Created by hi1311 on 2017/5/2.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#ifndef TCPClientSocket_h
#define TCPClientSocket_h
#import "GCDAsyncSocket.h"

#define GLobalTCPClientSocket [TCPClientSocket sharedInstance]

@interface TCPClientSocket : NSObject
+ (instancetype)sharedInstance;
- (BOOL)connect;
- (void)disConnect;
- (void)sendMsg:(NSString *)msg;
@end

#endif /* TCPClientSocket_h */
