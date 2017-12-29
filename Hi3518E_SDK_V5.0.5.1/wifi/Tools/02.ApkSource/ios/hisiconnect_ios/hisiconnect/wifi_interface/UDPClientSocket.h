//
//  UDPClientSocket.h
//  hisiconnect
//
//  Created by hi1311 on 2017/5/2.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#ifndef UDPClientSocket_h
#define UDPClientSocket_h

#import "GCDAsyncUdpSocket.h"



@protocol UDPClientSocketDelegate <NSObject>
- (void)clientSocketDidReceiveMessage:(NSString *)message;
@end

@interface UDPClientSocket : NSObject <GCDAsyncUdpSocketDelegate>

@property (nonatomic,assign)NSInteger times;

@property (nonatomic, strong)GCDAsyncUdpSocket * udpSocket;

-(void)createUDPClientSocket;
- (void)sendMessageToHost:(NSString *)targetIp:(unsigned int)targetPort:(NSData *)data;
- (void)disconnect;

@end



#endif /* UDPClientSocket_h */
