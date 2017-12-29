//
//  TCPServerSocket.h
//  hisiconnect
//
//  Created by hi1311 on 2017/5/2.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#ifndef TCPServerSocket_h
#define TCPServerSocket_h
#import "GCDAsyncSocket.h"

@interface TCPServerSocket : NSObject <GCDAsyncSocketDelegate>
@property (strong, nonatomic)GCDAsyncSocket * serverSocket;
@property (strong, nonatomic)GCDAsyncSocket * clientSocket;

-(void)createServerSocket:(dispatch_queue_t)queue:(SEL) processMessageFun;
@end
#endif /* TCPServerSocket_h */
