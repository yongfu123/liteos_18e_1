//
//  UDPClientSocket.m
//  hisiconnect
//
//  Created by hi1311 on 2017/5/2.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "UDPClientSocket.h"

//static  NSString * hostIp = @"192.168.1.2";
//static const uint16_t hostPort = 1234;

@interface UDPClientSocket ()

@end


@implementation UDPClientSocket

-(void)createUDPClientSocket
{
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    self.udpSocket = [[GCDAsyncUdpSocket alloc]initWithDelegate:self delegateQueue:queue];
    [self.udpSocket enableBroadcast:YES error:nil];
}

- (void)sendMessageToHost:(NSString *)targetIp:(unsigned int)targetPort:(NSData *)data
{
    //NSData * data = [message dataUsingEncoding:NSUTF8StringEncoding];
    
    //NSLog(@"sendMessageToHost = %@ %@",targetIp,data);
    
    // 根据需求写自己监听的端口
    [self.udpSocket sendData:data toHost:targetIp port:targetPort withTimeout:-1 tag:0];
    // 只接收一次  建议这行注释  因为一般是有数据回来就接收
    //[self.udpSocket receiveOnce:nil];
}

- (void)disconnect{
    [self.udpSocket closeAfterSending];
}

- (void)udpSocket:(GCDAsyncUdpSocket *)sock didReceiveData:(NSData *)data fromAddress:(NSData *)address withFilterContext:(id)filterContext{
    //NSString * ip = [GCDAsyncUdpSocket hostFromAddress:address];
    //uint16_t port = [GCDAsyncUdpSocket portFromAddress:address];
    
    //NSString * message = [[NSString alloc]initWithData:data encoding:NSUTF8StringEncoding];
    
    //[self.udpSocket receiveOnce:nil];
    [self.udpSocket beginReceiving:nil];
}


- (void)udpSocket:(GCDAsyncUdpSocket *)sock didNotSendDataWithTag:(long)tag dueToError:(NSError *)error{
    NSLog(@"udp client didNotSendDataWithTag  %@",[error description]);
}


- (void)udpSocket:(GCDAsyncUdpSocket *)sock didSendDataWithTag:(long)tag{
    //NSLog(@"客户端发送消息成功");
    self.times ++;
}

- (void)udpSocketDidClose:(GCDAsyncUdpSocket *)sock withError:(NSError *)error{
    NSLog(@"udp client udpSocketDidClose   %@",[error description]);
}

@end
