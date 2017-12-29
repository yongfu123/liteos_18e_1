//
//  TCPServerSocket.m
//  hisiconnect
//
//  Created by hi1311 on 2017/5/2.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TCPServerSocket.h"
#import "hisi_util.h"

static const uint16_t hostPort = 0x3516;
#define WELCOME_MSG  0

#define READ_TIMEOUT 40.0

@interface TCPServerSocket()
{
    SEL *processMessageFun;
}
@end

@implementation TCPServerSocket
-(void)createServerSocket:(dispatch_queue_t)queue:(SEL) processMessageFun
{
    NSLog(@"$tcp server createServerSocket thread = %@", [NSThread currentThread]);
    self.serverSocket = [[GCDAsyncSocket alloc]initWithDelegate:self delegateQueue:queue];
    processMessageFun = processMessageFun;
    
    NSLog(@"createServerSocket,port:%d",hostPort);
    NSError * error = nil;
    [self.serverSocket setAutoDisconnectOnClosedReadStream:NO];
    [self.serverSocket acceptOnPort:hostPort error:&error];
}

#pragma mark- GCDAsyncSocketDelegate
- (void)socket:(GCDAsyncSocket *)sock didAcceptNewSocket:(GCDAsyncSocket *)newSocket
{
    //连接成功，可查看newSocket.connectedHost和newSocket.connectedPort等参数
    self.clientSocket = newSocket;
   
    [newSocket readDataToLength:6 withTimeout:-1 tag:0];
}

- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
    NSString * receive = [[NSString alloc]initWithData:data encoding:NSUTF8StringEncoding];
    
    //self.recieveTextView.text = [NSString stringWithFormat:@"%@\n%@",self.recieveTextView.text,receive];
    NSLog(@"$$$$$$$tcp server didReadData:%@",receive);
    [[NSNotificationCenter defaultCenter] postNotificationName:OnlineNSNotificationName object:nil userInfo:@{@"message":[NSString stringWithFormat:@"%d",MSG_ONLINE_RECEIVED]}];
}

- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err;
{
    NSLog(@"tcp server socketDidDisconnect,err:%@",err);
}

- (void)socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag
{
    NSLog(@"didWriteDataWithTag  %ld",tag);
}

- (NSTimeInterval)socket:(GCDAsyncSocket *)sock shouldTimeoutReadWithTag:(long)tag
                 elapsed:(NSTimeInterval)elapsed
               bytesDone:(NSUInteger)length
{
    NSLog(@"shouldTimeoutReadWithTag  %ld",tag);
    if (elapsed <= READ_TIMEOUT)
    {
        NSString *warningMsg = @"Are you still there?\r\n";
        NSData *warningData = [warningMsg dataUsingEncoding:NSUTF8StringEncoding];

        [self.clientSocket writeData:warningData withTimeout:-1 tag:0];
            return 0;
    }
    
    return 0.0;
    
}
@end
