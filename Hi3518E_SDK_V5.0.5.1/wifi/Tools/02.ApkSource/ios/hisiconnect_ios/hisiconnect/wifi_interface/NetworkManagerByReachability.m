//
//  NetworkManagerByReachability.m
//  hisiconnect
//
//  Created by hi1311 on 2017/4/24.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "NetworkManagerByReachability.h"
#import "Reachability.h"
#import <SystemConfiguration/SystemConfiguration.h>
#import <SystemConfiguration/CaptiveNetwork.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>


@interface NetworkManagerByReachability()

@property (nonatomic) Reachability *hostReachability;

@end

@implementation NetworkManagerByReachability

+(instancetype)sharedInstance
{
    static NetworkManagerByReachability *networkManager;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        networkManager = [[NetworkManagerByReachability alloc] init];
    });
    
    return networkManager;
}


//network test
/*!
 * Called by Reachability whenever status changes.
 */
- (void) reachabilityChanged:(NSNotification *)note
{
    Reachability* curReach = [note object];
    NSParameterAssert([curReach isKindOfClass:[Reachability class]]);
    [self updateInterfaceWithReachability:curReach];
}


- (void)updateInterfaceWithReachability:(Reachability *)reachability
{
    if (reachability == self.hostReachability)
    {
        //[self configureTextField:self.remoteHostStatusField imageView:self.remoteHostImageView reachability:reachability];
        
        NetworkStatus netStatus = [reachability currentReachabilityStatus];
        BOOL connectionRequired = [reachability connectionRequired];
        
        
        NSLog(@"updateInterfaceWithReachability netStatus:%d",netStatus);
        NSLog(@"updateInterfaceWithReachability connectionRequired:%d",connectionRequired);
    }
}

- (NSString *)getLocalIPAddress:(NSString*) networkName
{
    NSString       *localIP = nil;
    struct ifaddrs *addrs;
    
    if (getifaddrs(&addrs)==0) {
        const struct ifaddrs *cursor = addrs;
        while (cursor != NULL) {
            if (cursor->ifa_addr->sa_family == AF_INET && (cursor->ifa_flags & IFF_LOOPBACK) == 0)
            {
                NSString *name = [NSString stringWithUTF8String:cursor->ifa_name];
                if ([name isEqualToString:networkName])
                {
                    localIP = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)cursor->ifa_addr)->sin_addr)];
                    break;
                }
            }
            cursor = cursor->ifa_next;
        }
        freeifaddrs(addrs);
    }
    return localIP;
}

-(NSString*) getWifiMACAddress
{
    CFArrayRef wifiInterfaces = CNCopySupportedInterfaces();
    if(!wifiInterfaces){
        NSLog(@"fale.....");
    }
    
    NSArray*interfaces = (__bridge NSArray*)wifiInterfaces;
    NSString* dic = nil;

    for (NSString*interfaceName in interfaces) {
        CFDictionaryRef dictRef = CNCopyCurrentNetworkInfo((__bridge CFStringRef)(interfaceName));
        if (dictRef) {
            NSDictionary *networkInfo = (__bridge NSDictionary *)dictRef;
            //NSLog(@"network info -> %@", networkInfo);
            dic = [networkInfo objectForKey:@"BSSID"];
        }
    }
    CFRelease(wifiInterfaces);
    return dic;
}

-(NSString*) getWifiSSID
{
    CFArrayRef wifiInterfaces = CNCopySupportedInterfaces();
    if(!wifiInterfaces){
        NSLog(@"fale.....");
    }
    
    NSArray*interfaces = (__bridge NSArray*)wifiInterfaces;
    NSString* dic = nil;
    
    for (NSString*interfaceName in interfaces) {
        CFDictionaryRef dictRef = CNCopyCurrentNetworkInfo((__bridge CFStringRef)(interfaceName));
        if (dictRef) {
            NSDictionary *networkInfo = (__bridge NSDictionary *)dictRef;
            //NSLog(@"network info -> %@", networkInfo);
            dic = [networkInfo objectForKey:@"SSID"];
        }
    }
    CFRelease(wifiInterfaces);
    return dic;
}

- (BOOL)isWiFiEnabled {
    NSCountedSet * cset = [NSCountedSet new];
    struct ifaddrs *interfaces;
    if(!getifaddrs(&interfaces)) {
        for( struct ifaddrs *interface = interfaces; interface; interface = interface->ifa_next)
        {
            if ( (interface->ifa_flags & IFF_UP) == IFF_UP ) {
                [cset addObject:[NSString stringWithUTF8String:interface->ifa_name]];
                //NSLog(@"interface->ifa_name:%s", interface->ifa_name);
            }
        }
    }
    return [cset countForObject:@"awdl0"] > 1 ? YES : NO;
}

@end
