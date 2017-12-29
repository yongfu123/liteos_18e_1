//
//  hisi_util.h
//  hisiconnect
//
//  Created by hi1311 on 2017/5/17.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#ifndef hisi_util_h
#define hisi_util_h

//the proto which online message is received by
static const int ONLINE_MSG_BY_UDP        = 0;
static const int ONLINE_MSG_BY_TCP        = 1;
static const int ONLINE_PORT_BY_UDP       = 1131;


//handler message
static const int MSG_ONLINE_RECEIVED      = 0;
static const int MSG_MULTICAST_TIMEOUT    = 1;
static const int MSG_AP_RECEIVED_ACK      = 2;
static const int MSG_CONNECTED_APMODE     = 3;
static const int MSG_APMODE_TIMEOUT       = 4;

static NSString *OnlineNSNotificationName = @"onlineNSNotification";

#endif /* hisi_util_h */
