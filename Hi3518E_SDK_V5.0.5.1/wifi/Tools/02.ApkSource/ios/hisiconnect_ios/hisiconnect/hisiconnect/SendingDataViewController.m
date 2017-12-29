//
//  SendingDataViewController.m
//  hisiconnect
//
//  Created by hi1311 on 2017/4/24.
//  Copyright © 2017年 com.huawei.hisiconnect. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SendingDataViewController.h"
#import "UDPClientSocket.h"
#import "ProcessMultiCastLinkData.h"
#import "WifiNetworkInfo.h"
#import "NetworkManagerByReachability.h"
#import "hisi_util.h"
#import "DropdownMenu.h"


static int TIMER_MULTICAST_TIMEOUT        = 60;//60s
//static int TIMER_APMODE_TIMEOUT           = 60;//60s

int        counterTime = 0;
static int ONLINE_PORT = 0x3516;

@interface SendingDataViewController() <UITableViewDelegate,UITableViewDataSource>
{

    UITableView      *tableView;
    //创建数据源数组
    NSMutableArray   *dataArry;
    UILabel          *deviceIDLable;
    UITextField      *deviceIDTextField;
    UILabel          *wifiProtoLable;
    DropdownMenu     *dropdownMenu;
    UILabel          *passwordLable;
    UITextField      *passwordTextField;
    UIButton         *sendDataButton;
    UILabel          *linkresultLable;
    
    NSTimer          *sendTimeOutTimer;
    WifiNetworkInfo  *myWifiNetworkInfo;

    Boolean           multiCastButtonSending;
    
    GCDAsyncSocket          *onlineServerSocket;
    ProcessMultiCastLinkData *multiCastLinkDataThread;
}
@end


@implementation SendingDataViewController

@synthesize connectModel;
@synthesize connectAPInfo;


UITableView    *tableView;
//创建数据源数组
NSMutableArray *dataArry;

- (void)viewDidLoad {
    
    [super viewDidLoad];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicantionWillResignActive:) name:UIApplicationWillResignActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicantionDidBecomActive:) name:UIApplicationDidBecomeActiveNotification object:nil];
    [self checkWifiNetwork];
    /*---create table view begin-----*/
    [self createData];
    [self createTableView];
    /*---create table view end-----*/
    
    NSLog(@"ssid:%@",connectAPInfo.ssid);
    NSLog(@"bssid:%@",connectAPInfo.bssid);

    
    /*---create wifi drop down menu begin*/
    [self createWifiProtoView];
    /*---create wifi drop down menu end*/
    
    /*---create wifi password textfield begin*/
    [self createWifiPasswordInfoView];
    /*---create wifi password textfield end*/
    
    /*---create device id info begin*/
    [self createDeviceIDInfoView];
    /*---create device id info end*/
    
    [self createSendingButtonView];
    
    [self createLinkResultInfoView];
    
    myWifiNetworkInfo = [[WifiNetworkInfo alloc]init];
    myWifiNetworkInfo.security = -1;
}

-(void) handleMessage:(const int)value {
    
    switch(value){
        case MSG_ONLINE_RECEIVED:
            NSLog(@"process notification MSG_ONLINE_RECEIVED");
            [sendTimeOutTimer invalidate];
            sendTimeOutTimer =nil;
            //onlineRecieveTime = System.currentTimeMillis();
            //long castTime = onlineRecieveTime-buttonPressTime;
            //停止接收上线包
            [self stopServerSocket];

            //停止广播
            [GLobalProcessMultiCastLinkData  stopSendMultiBroadcast];
            //multicastTimer.cancel();
            [sendTimeOutTimer invalidate];
            sendTimeOutTimer = nil;
        
            sendDataButton.enabled = true;
            multiCastButtonSending =false;
            [sendDataButton setTitle:@"发送" forState:UIControlStateNormal];
            linkresultLable.textColor = [UIColor redColor];
            linkresultLable.text= @"恭喜！可以快乐的玩耍了\n 超过了全国99.9%的用户!";
            break;
        case MSG_MULTICAST_TIMEOUT:
            NSLog(@"process notification MSG_MULTICAST_TIMEOUT");
            [sendTimeOutTimer invalidate];
            sendTimeOutTimer =nil;
            
            //停止接受上线包
            [self stopServerSocket];
            //停止广播
            [GLobalProcessMultiCastLinkData  stopSendMultiBroadcast];
            [sendTimeOutTimer invalidate];
            sendTimeOutTimer = nil;
            
            sendDataButton.enabled = true;
            multiCastButtonSending =false;
            [sendDataButton setTitle:@"发送" forState:UIControlStateNormal];
            linkresultLable.textColor = [UIColor redColor];
            linkresultLable.text= @"连接失败，请确认一下内容\n 1. 手机已连接家中的Wi-Fi.\n 2. 输入的密码正确.\n 3. 家中的路由器工作在2.4G模式下.\n 若重试后仍然失败，请尝试点击AP模式关联.";
            break;
            
        //AP 收到单播信息
        case MSG_AP_RECEIVED_ACK:
            break;
        case MSG_CONNECTED_APMODE:
            break;
        case MSG_APMODE_TIMEOUT:
            break;
        default:
            break;
    }
    
}

-(void)multiCastTimerOutProcess {
    NSLog(@"delayMethod");
    //[[NSNotificationCenter defaultCenter] postNotificationName:OnlineNSNotificationName object:nil userInfo:@{@"message":[NSString stringWithFormat:@"%d",MSG_MULTICAST_TIMEOUT]}];
    [self handleMessage:MSG_MULTICAST_TIMEOUT];
}


-(void)processAPCastButtonEvent{
    NSLog(@"Button clicked.AP模式");
    [self showAlertMessage:@"AP模式不支持"];
}

/*online thread process begin*/
-(void)createServerSocket
{
    NSLog(@"$tcp server createServerSocket thread = %@", [NSThread currentThread]);
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
    self.serverSocket = [[GCDAsyncSocket alloc]initWithDelegate:self delegateQueue:queue];
    
    NSLog(@"$$$$$$createServerSocket,port:%d",hostPort);
    NSError * error = nil;
    [self.serverSocket setAutoDisconnectOnClosedReadStream:NO];
    [self.serverSocket acceptOnPort:hostPort error:&error];
    
    NSLog(@"createServerSocket,error:%@",error);
}

-(void)stopServerSocket
{
    [self.serverSocket disconnect];
    self.serverSocket = nil;
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
    
    NSLog(@"$$$$$$$tcp server didReadData:%@",receive);
    [[NSNotificationCenter defaultCenter] postNotificationName:OnlineNSNotificationName object:nil userInfo:@{@"message":[NSString stringWithFormat:@"%d",MSG_ONLINE_RECEIVED]}];

    [self handleMessage:MSG_ONLINE_RECEIVED];
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
    if (elapsed <= READ_TIMEOUT)
    {
        NSString *warningMsg = @"Are you still there?\r\n";
        NSData *warningData = [warningMsg dataUsingEncoding:NSUTF8StringEncoding];
        
        [self.clientSocket writeData:warningData withTimeout:-1 tag:0];
        return 0;
    }

    return 0;
}

-(void)recieveOnlineMessage{
    // 初始化
    //self.notifications             = [[NSMutableArray alloc] init];
    //self.notificationLock          = [[NSLock alloc] init];
    //self.notificationThread        = [NSThread currentThread];
    //self.notificationPort          = [[NSMachPort alloc] init];
    //self.notificationPort.delegate = self;
    
    // 往当前线程的run loop添加端口源
    // 当Mach消息到达而接收线程的run loop没有运行时，则内核会保存这条消息，直到下一次进入run loop
    //[[NSRunLoop currentRunLoop] addPort:self.notificationPort
    //                            forMode:(__bridge NSString *)kCFRunLoopCommonModes];
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(processNotification:) name:OnlineNSNotificationName object:nil];
    //[[NSNotificationCenter defaultCenter] postNotificationName:OnlineNSNotificationName object:nil userInfo:@{@"message":[NSString stringWithFormat:@"%d",MSG_MULTICAST_TIMEOUT]}];
    //dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
    
    //});
    
    /*tcp server begin*/
    [self createServerSocket];
    /*tcp server end*/
}
/*online thread process end*/

- (void)showAlertMessage:(NSString *)alertMessage
{
    
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"警告" message:alertMessage preferredStyle:UIAlertControllerStyleAlert];
    
    // 添加按钮
    [alert addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        NSLog(@"点击了取消按钮");
    }]];
    
    [self presentViewController:alert animated:YES completion:nil];
}

-(Boolean)checkNetworkInfo:(WifiNetworkInfo *)networkInfo
{
    if(networkInfo.security !=SECURITY_NONE && networkInfo.security != SECURITY_WEP && networkInfo.security !=SECURITY_PSK)
    {
        [self showAlertMessage:@"安全性未选择"];
        NSLog(@"安全性未选择");
        return false;
    }
    
    if((networkInfo.security == SECURITY_PSK) && (passwordTextField.text.length <8 || passwordTextField.text.length > 64))
    {
        NSLog(@"WPA/WPA2 PSK,密码长度为8～64");
        [self showAlertMessage:@"WPA/WPA2 PSK,密码长度为8～64"];
        return false;
    }
    
    if ((SECURITY_WEP == networkInfo.security) && (passwordTextField.text.length != 5) && (passwordTextField.text.length != 13)
        && (passwordTextField.text.length != 10) && (passwordTextField.text.length != 26))
    {
        NSLog(@"WEP,ASCII方式：密码长度为5或13，十六进制方式：密码长度为10或26");
        [self showAlertMessage:@"WEP,ASCII方式：密码长度为5或13，十六进制方式：密码长度为10或26"];
        return false;
    }
    
    if(deviceIDTextField.text.length !=4)
    {
        [self showAlertMessage:@"请输入四个字符的设备ID,该值为设备MAC后四个十六进制数，比如：**:**:FF:FF(输入FFFF)"];
        return false;
    }
    
    return true;
}

-(void)processStopMultiCast
{
    [sendTimeOutTimer invalidate];
    sendTimeOutTimer =nil;
    //停止接收上线包
    [self stopServerSocket];
    NSLog(@"Button multiCastButtonSending stop");
    [sendDataButton setTitle:@"发送" forState:UIControlStateNormal];
    //停止广播
    [GLobalProcessMultiCastLinkData  stopSendMultiBroadcast];
    [sendTimeOutTimer invalidate];
    sendTimeOutTimer = nil;
    
    multiCastButtonSending = false;
    linkresultLable.text= @"成功停止组播发送";
}

-(void)processMultiCastButtonEvent{
    NSLog(@"Button clicked.组播模式");
    NSLog(@"Button clicked.device: %@",deviceIDTextField.text);
    
    Boolean flag =[self checkNetworkInfo:myWifiNetworkInfo];
    if(flag != true)
    {
        return;
    }
    
    NSLog(@"Button multiCastButtonSending: %d",multiCastButtonSending);
    if(multiCastButtonSending != false)/*发送时，stop发送*/
    {
        [self processStopMultiCast];
        return;
    }
    
    multiCastButtonSending = true;
    linkresultLable.textColor = [UIColor blackColor];
    linkresultLable.text= @"组播发送中.......";
    
    [sendDataButton setTitle:@"停止发送" forState:UIControlStateNormal];
    
    /*add time out timer*/
    sendTimeOutTimer  = [NSTimer scheduledTimerWithTimeInterval:TIMER_MULTICAST_TIMEOUT target:self selector:@selector(multiCastTimerOutProcess) userInfo:nil repeats:NO];
    

    [myWifiNetworkInfo setSsid:[GLobalNetworkManagerByReachability getWifiSSID]];
    
    if((myWifiNetworkInfo.security==SECURITY_WEP) || (myWifiNetworkInfo.security==SECURITY_PSK))
    {
        /*get password*/
        NSString *password = passwordTextField.text;
        NSLog(@"Button clicked.password:%@",password);
        [myWifiNetworkInfo setPassword:password];
    }
    else
    {
        [myWifiNetworkInfo setPassword:@""];
    }
    
    [myWifiNetworkInfo setSecurity:myWifiNetworkInfo.security];
    [myWifiNetworkInfo setDeviceName:deviceIDTextField.text];
    [myWifiNetworkInfo setIp:[GLobalNetworkManagerByReachability getLocalIPAddress:@"en0"]];
    [myWifiNetworkInfo setPort:ONLINE_PORT];
    [myWifiNetworkInfo setOnlineProto:ONLINE_MSG_BY_TCP];
    
    /*构建数据包*/
    [GLobalProcessMultiCastLinkData  constructMultiCastLinkMessageToSend:myWifiNetworkInfo];
    
    /*创建线程监听上线消息*/
    [self recieveOnlineMessage];
    
    /*发送报文*/
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        [GLobalProcessMultiCastLinkData  startSendMultiBroadcast:true:3516];
    });
    
    NSLog(@"finish processMultiCastButtonEvent");
}

//MyView中的按钮的事件
- (IBAction)SendDataBtnClick:(UIButton *) sendDataButton
{
    NSLog(@"Method in controller.");
    NSLog(@"Button clicked.");

    /*udp begin ----ok*/
    //UDPClientSocket * clientSocekt = [[UDPClientSocket alloc]init];
    //clientSocekt.delegate = self;
    
    // 这里服务器的ip写自己所要连接的服务端ip
    //[clientSocekt sendMessageToHost:@"192.168.1.2"];
    //[clientSocekt disconnect];
    /*udp end------ok*/
    
    /*tcp server begin*/
    //TCPServerSocket *serverSocket = [[TCPServerSocket alloc]init];
    //serverSocket = [[TCPServerSocket alloc]init];
    //[serverSocket createServerSocket];
    /*tcp server end*/

    /*tcp client begin------ok*/
    //[GLobalTCPClientSocket connect];
    //[GLobalTCPClientSocket sendMsg:@"tcp client\r\n"];
    /*tcp client end------ok*/
    
    if([connectModel isEqualToString:@"AP模式"])
    {
        [self processAPCastButtonEvent];
    }
    else if([connectModel isEqualToString:@"组播模式"])
    {
        [self processMultiCastButtonEvent];
    }
}

/*---------------------add table view begin--------------------------*/
-(void)createData{
    dataArry=[[NSMutableArray alloc]init];
    //创建一维数组存储每个分区的cell数据
    NSMutableArray *arr=[[NSMutableArray alloc]init];
    
    //存储每一行cell的数据，每一行的cell的数据模型
    NSString *ssidStr =[NSString stringWithFormat:@"SSID:   %@",[GLobalNetworkManagerByReachability getWifiSSID]];
    [arr addObject:ssidStr];
    NSString *bssidStr =[NSString stringWithFormat:@"BSSID:  %@",[GLobalNetworkManagerByReachability getWifiMACAddress]];
    [arr addObject:bssidStr];
    
    [dataArry addObject:arr];
}
-(void)createTableView{
     self.automaticallyAdjustsScrollViewInsets = NO;
    
    tableView =[[UITableView alloc]initWithFrame:CGRectMake(0, 64, self.view.frame.size.width, self.view.frame.size.height/4 ) style:UITableViewStylePlain];
    //设置数据源
    tableView.dataSource =self;
    //设置代理
    tableView.delegate = self;
    tableView.scrollEnabled =false;
    tableView.allowsSelection = false;
    
    [tableView setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    [self.view addSubview:tableView];
}
#pragma mark - UITableViewDataSource协议
//设置有多少分区
-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return dataArry.count;
}
//每个分区有多少行
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [dataArry[section] count];
}
//获取cell  每次显示cell 之前都要调用这个方法
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    //创建复用标识符
    static NSString *identifire = @"identifier";
    UITableViewCell *cell =[tableView dequeueReusableCellWithIdentifier:identifire];
    if (!cell) {//如果没有可以复用的
        cell =[[UITableViewCell alloc]initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifire];
    }
    //填充cell  把数据模型中的存储数据 填充到cell中
    cell.textLabel.text=dataArry[indexPath.section][indexPath.row];
    return cell;
}
//设置头标
-(NSString*)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
    return [NSString stringWithFormat:@"已连接热点信息"];
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*------------------------add table view end-----------------------------*/


/*--------------------add wifi down menu begin -------------------*/

-(void)createWifiProtoView
{
    wifiProtoLable=[[UILabel alloc] initWithFrame:CGRectMake(15, 70+self.view.frame.size.height/4, 80, 30)];
    wifiProtoLable.text=@"安全性:";
    
    [self.view addSubview:wifiProtoLable];
    
    dropdownMenu = [[DropdownMenu alloc] init];
    [dropdownMenu setFrame:CGRectMake(100, 70+self.view.frame.size.height/4, 200, 30)];
    [dropdownMenu setMenuTitles:@[@"开放",@"WEP",@"WPA/WPA2 PSK"] rowHeight:30];
    dropdownMenu.delegate = self;
    [self.view addSubview:dropdownMenu];
}


- (void)dropdownMenu:(DropdownMenu *)menu selectedCellNumber:(NSInteger)number{
    switch (number) {
        case 0:/*OPEN*/
            [myWifiNetworkInfo setSecurity:SECURITY_NONE];
            passwordTextField.enabled = false;
            passwordTextField.text =@"";
            break;
        case 1:/*WEP*/
            passwordTextField.enabled = true;
            [myWifiNetworkInfo setSecurity:SECURITY_WEP];
            break;
        case 2:/*WPA/WPA2 PSK*/
            passwordTextField.enabled = true;
            [myWifiNetworkInfo setSecurity:SECURITY_PSK];
            break;
        default:
            break;
    }
}

/*--------------------add wifi down menu end -------------------*/

/*--------------------add wifi password start-------------------*/
-(void)createWifiPasswordInfoView
{
    passwordLable=[[UILabel alloc] initWithFrame:CGRectMake(15, 120+self.view.frame.size.height/4, 80, 30)];
    passwordLable.text=@"密码:";
    
    [self.view addSubview:passwordLable];
    passwordTextField=[[UITextField alloc] initWithFrame:CGRectMake(100, 120+self.view.frame.size.height/4, 200, 30)];//初始化坐标位置
    //deviceIDTextField.placeholder = @"ffff";//为空白文本字段绘制一个灰色字符串作为占位符

    passwordTextField.secureTextEntry = YES;
    passwordTextField.textAlignment = UITextLayoutDirectionLeft;//默认就是左对齐，这个是UITextField扩展属性
    
    passwordTextField.borderStyle=UITextLayoutDirectionLeft;  //设置textField的形状
    
    //deviceIDTextField.clearsOnBeginEditing = NO;//设置为YES当用点触文本字段时，字段内容会被清除,这个属性一般用于密码设置，当输入有误时情况textField中的内容
    
    passwordTextField.returnKeyType=UIReturnKeyDone;//设置键盘完成按钮
    
    passwordTextField.delegate=self;//委托类需要遵守UITextFieldDelegate协议
    
    passwordTextField.font = [UIFont systemFontOfSize:18]; //设置TextFiel输入框字体大小
    passwordTextField.enabled=false;
    
    [self.view addSubview:passwordTextField];
    
}
/*--------------------add wifi password start-------------------*/



/*------------------------add device id info begin-----------------------*/
-(void)createDeviceIDInfoView
{
    deviceIDLable=[[UILabel alloc] initWithFrame:CGRectMake(15, 170+self.view.frame.size.height/4, 80, 30)];
    deviceIDLable.text=@"设备ID:";
    
    [self.view addSubview:deviceIDLable];
    deviceIDTextField=[[UITextField alloc] initWithFrame:CGRectMake(100, 170+self.view.frame.size.height/4, 200, 30)];//初始化坐标位置
    //deviceIDTextField.placeholder = @"ffff";//为空白文本字段绘制一个灰色字符串作为占位符
    deviceIDTextField.text =@"ffff";
    
    deviceIDTextField.textAlignment = UITextLayoutDirectionLeft;//默认就是左对齐，这个是UITextField扩展属性
    
    deviceIDTextField.borderStyle=UITextLayoutDirectionLeft;  //设置textField的形状
    
    //deviceIDTextField.clearsOnBeginEditing = NO;//设置为YES当用点触文本字段时，字段内容会被清除,这个属性一般用于密码设置，当输入有误时情况textField中的内容
    
    deviceIDTextField.returnKeyType=UIReturnKeyDone;//设置键盘完成按钮
    
    deviceIDTextField.delegate=self;//委托类需要遵守UITextFieldDelegate协议
    
    deviceIDTextField.font = [UIFont systemFontOfSize:18]; //设置TextFiel输入框字体大小
    
    [self.view addSubview:deviceIDTextField];
    
}

-(BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string{
    
    if (textField == deviceIDTextField) {
        NSUInteger lengthOfString = string.length;  //lengthOfString的值始终为1
        for (NSInteger loopIndex = 0; loopIndex < lengthOfString; loopIndex++) {
            unichar character = [string characterAtIndex:loopIndex]; //将输入的值转化为ASCII值（即内部索引值），可以参考ASCII表
            // 48-57;{0,9};65-90;{A..Z};97-122:{a..z}
            if (character < 48) return NO; // 48 unichar for 0
            if (character > 57 && character < 65) return NO; //
            if (character > 90 && character < 97) return NO;
            if (character > 122) return NO;
            
        }
        // Check for total length
        NSUInteger proposedNewLength = textField.text.length - range.length + string.length;
        if (proposedNewLength > 4) {
            return NO;//限制长度
        }
        return YES;
        
    }
    return YES;
}

//按下Done按钮时调用这个方法，可让按钮消失
-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
    
    [textField resignFirstResponder];
    return YES;
}
/*--------------------add device id info end----------------------*/

/*------------------------add send button begin-----------------------*/
-(void)createSendingButtonView
{
    sendDataButton =[[UIButton alloc] initWithFrame:CGRectMake(self.view.frame.size.width/2-50, 220+self.view.frame.size.height/4, 100, 30)];
    [sendDataButton setTitle:@"发送" forState:UIControlStateNormal];
    [sendDataButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    sendDataButton.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
    [sendDataButton.layer setBorderWidth:2];
    [sendDataButton.layer setCornerRadius:10];
    [sendDataButton.layer setMasksToBounds:YES];
    sendDataButton.enabled=true;
    
    [sendDataButton addTarget:self action:@selector(SendDataBtnClick:) forControlEvents: UIControlEventTouchUpInside];
    [self.view addSubview:sendDataButton];
}

/*------------------------add send button end-----------------------*/

/*------------------------add result label begin--------------------*/
-(void)createLinkResultInfoView
{
    linkresultLable=[[UILabel alloc] initWithFrame:CGRectMake(15, 240+self.view.frame.size.height/4, 300, 200)];
    linkresultLable.font =[UIFont systemFontOfSize:15.0];
    linkresultLable.text=@"";
    linkresultLable.lineBreakMode = UILineBreakModeCharacterWrap;
    linkresultLable.textAlignment =
    linkresultLable.numberOfLines =0;
    
    
    [self.view addSubview:linkresultLable];
}
/*------------------------add result label end--------------------*/



- (void)wifiNetworkUnreachalertController:(NSString *)alertMessage
{
    // 危险操作:弹框提醒
    // 1.UIAlertView
    // 2.UIActionSheet
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"警告" message:alertMessage preferredStyle:UIAlertControllerStyleAlert];
    
    [alert addAction:[UIAlertAction actionWithTitle:@"前往" style:UIAlertActionStyleDestructive handler:^(UIAlertAction *action) {
        NSURL *url = [NSURL URLWithString:@"Prefs:root=WIFI"];
        
        [[UIApplication sharedApplication] openURL:url];
        
        /*used in ios10*/
        //Class LSApplicationWorkspace = NSClassFromString(@"LSApplicationWorkspace");
        //[[LSApplicationWorkspace performSelector:@selector(defaultWorkspace)] performSelector:@selector(openSensitiveURL:withOptions:) withObject:url withObject:nil];
    }]];
    [alert addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        NSLog(@"点击了取消按钮");
        exit(0);
    }]];
    
    [self presentViewController:alert animated:YES completion:nil];
}

-(void)checkWifiNetwork {
    BOOL isWIFI =[GLobalNetworkManagerByReachability isWiFiEnabled];
    if (!isWIFI) {//如果WiFi没有打开，作出弹窗提示
        [self wifiNetworkUnreachalertController:@"Wi-Fi未打开，继续操作需要打开Wi-Fi，是否前往？"];
    }
    
    NSString *connectedSSID = [GLobalNetworkManagerByReachability getWifiSSID];
    NSString *connectedWifiIP = [GLobalNetworkManagerByReachability getLocalIPAddress:@"en0"];
    if(connectedSSID == nil || connectedWifiIP == nil)
    {
        [self wifiNetworkUnreachalertController:@"Wi-Fi未关联，继续操作需要打开Wi-Fi进行关联，是否前往？"];
    }
}

-(void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

-(void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    NSLog(@"viewWillAppear");
    [self checkWifiNetwork];
    UILabel *titleLab = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 200, 100)];
    titleLab.text             = connectModel;
    titleLab.font             = [UIFont boldSystemFontOfSize:18.f];
    titleLab.textAlignment    = NSTextAlignmentCenter;
    titleLab.textColor        = [UIColor blackColor];
    self.navigationItem.titleView = titleLab;
}

-(void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    NSLog(@"viewWillDisappear");
    [self processStopMultiCast];
}


-(void)applicantionWillResignActive:(NSNotification *)notification
{
    NSLog(@"applicantionWillResignActive");
}

-(void)applicantionDidBecomActive:(NSNotification *)notification
{
    NSLog(@"applicantionDidBecomActive");
    [self checkWifiNetwork];
    [tableView removeFromSuperview];
    [self createData];
    [self createTableView];
}

@end
