package com.hisilicon.hisilink;

import java.io.IOException;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Timer;
import java.util.TimerTask;

import com.huawei.hi1131s.hisilink.api.ProcessAPLinkData;
import com.huawei.hi1131s.hisilink.api.ProcessMultiCastLinkData;
import com.huawei.hi1131s.hisilink.api.WifiNetworkInfo;

import android.net.wifi.WifiConfiguration;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.ActionBar;
import android.app.Activity;
import android.graphics.drawable.ColorDrawable;
import android.text.method.HideReturnsTransformationMethod;
import android.text.method.PasswordTransformationMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.TextView;

public class DeviceActivity extends Activity {
	private static final String TAG = "DeviceActivity";
	//handler 消息
    private static final int MSG_ONLINE_RECEIVED = 0;
    private static final int MSG_MULTICAST_TIMEOUT = 1;
    private static final int MSG_AP_RECEIVED_ACK = 2;
    private static final int MSG_CONNECTED_APMODE = 3;
    private static final int MSG_APMODE_TIMEOUT = 4;
    //上线消息接收方式
    private static final int ONLINE_MSG_BY_UDP = 0;
    private static final int ONLINE_MSG_BY_TCP = 1;
    private static final int ONLINE_PORT_BY_UDP = 1131;
    //超时时间
    private static final int TIMER_MULTICAST_TIMEOUT = 60000;//45s
    private static final int TIMER_APMODE_TIMEOUT = 60000;//120s
    private int counterTime = 0;
    
    static final int SECURITY_ERR = 4; 
    private long buttonPressTime = -1;
    private long APModeStartTime = -1;
    private long onlineRecieveTime = -1;
	private WiFiAdmin mWiFiAdmin = null;
	private MessageSend mMessageSend = null;
	private int udpPort = 0;
	private String strName = null;
	private String SSID = null;
	private String onlineMessage = null;
	private String ackMessage = null;
	private int homeWifiID = -1;

	private Socket TCPSocket = null;
	private OutputStream outputStream=null;

	private boolean isBroadcastListening = true;
	private TextView errorHint = null;
	private Timer multicastTimer = null;
	private Timer APModeTimer = null;
	private Button buttonConnectAP = null;
	private Button buttonConnectMulti = null;
	private OnlineReciever onlineReciever = null;
	private EditText textPass;

   // static {
   //    System.loadLibrary("HisiLink");
    //}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_device);
		ActionBar actionBar = getActionBar();
		actionBar.setIcon(new ColorDrawable(getResources().getColor(android.R.color.transparent)));
		//获得MainActivity传递过来的设备名
		Bundle bundle = this.getIntent().getExtras();
		strName = bundle.getString("devicename");
		actionBar.setTitle(strName);
		SSID = bundle.getString("SSID");
		//上线消息
		constructOnlineMessage();
		TextView textSSID = (TextView)findViewById(R.id.textSSID);
		errorHint = (TextView)findViewById(R.id.errorhint);
		textPass = (EditText)findViewById(R.id.inputPass);
		buttonConnectAP = (Button)findViewById(R.id.connect_ap);
		buttonConnectMulti = (Button)findViewById(R.id.connect_multi);
		CheckBox checkBoxPassword = (CheckBox)findViewById(R.id.checkPassword);
		checkBoxPassword.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked){
                    //如果选中，显示密码      
                	textPass.setTransformationMethod(HideReturnsTransformationMethod.getInstance());
                	textPass.setSelection(textPass.getText().length());
                }else{
                    //否则隐藏密码
                	textPass.setTransformationMethod(PasswordTransformationMethod.getInstance());
                	textPass.setSelection(textPass.getText().length());
                }
            }
        });
		//取得手机关联的WiFi的SSID
		mWiFiAdmin = new WiFiAdmin(DeviceActivity.this);
		textSSID.setText(mWiFiAdmin.getWifiSSID());
	}

	@Override
	public void onDestroy() {
		if ((null != mWiFiAdmin)&&(null != SSID))
			mWiFiAdmin.forgetWifi(SSID);
	Log.e(TAG, "onDestroy");
	super.onDestroy();
	}

	public void onClick_Event_AP(View view){
		buttonConnectAP.setText("正在连接");
		buttonConnectAP.setClickable(false);
		buttonConnectMulti.setClickable(false);
		TCPSocket = null;
		APModeStartTime = System.currentTimeMillis();
		APModeTimer=new Timer();
		TimerTask APModetimeoutTask =new TimerTask() {
	        @Override
	        public void run() {
	            Message msg = handler.obtainMessage();
	            msg.what = MSG_APMODE_TIMEOUT;
	            handler.sendMessage(msg);
	        }
	    };
		//启动一个75s的定时器，超时提醒用户
		APModeTimer.schedule(APModetimeoutTask, TIMER_APMODE_TIMEOUT);
		
		ProcessAPLinkData processAPLinkData = new ProcessAPLinkData();
		WifiNetworkInfo wifiNetworkInfo = new WifiNetworkInfo();
		wifiNetworkInfo.setIp(mWiFiAdmin.getWifiIPAdress());
		wifiNetworkInfo.setSecurity(mWiFiAdmin.getSecurity());
		wifiNetworkInfo.setSsid(mWiFiAdmin.getWifiSSID());
		wifiNetworkInfo.setPassword(textPass.getText().toString());
		wifiNetworkInfo.setPort(1131);
		wifiNetworkInfo.setDeviceName(this.strName);
		wifiNetworkInfo.setOnlineProto(ONLINE_MSG_BY_UDP);
		processAPLinkData.constructAPLinkMessageToSend(wifiNetworkInfo);
		
		//配置设备AP网络信息
		WifiConfiguration mWifiConfig = new WifiConfiguration();
		mWifiConfig = constructWifiConfig(processAPLinkData,SSID);
		//保存路由器AP ID后续还要关联
		homeWifiID = mWiFiAdmin.getNetWorkId();
		Log.e(TAG, "homeWifiID="+homeWifiID);
		//断开路由器AP，关联设备AP
		mWiFiAdmin.disConnectionWifi(homeWifiID);
		mWiFiAdmin.addNetWork(mWifiConfig);

		//创建TCP连接
		TCPConnectThread connectThread = new TCPConnectThread(processAPLinkData);
		Log.d(TAG, "connectThread start");
		connectThread.start();
	}
	
	public void onClick_Event_Multi(View view){
		buttonConnectMulti.setText("正在连接");
		buttonConnectMulti.setClickable(false);
		buttonConnectAP.setClickable(false);
		buttonPressTime = System.currentTimeMillis();
		//再次点击时，清空下方提示信息
		errorHint.setText("");
		//启动一个45s的定时器，超时显示错误信息。
		multicastTimer=new Timer();
		TimerTask timeoutTask =new TimerTask() {
	        @Override
	        public void run() {
	            Message msg = handler.obtainMessage();
	            msg.what = MSG_MULTICAST_TIMEOUT;
	            handler.sendMessage(msg);
	        }
	    };
		multicastTimer.schedule(timeoutTask, TIMER_MULTICAST_TIMEOUT);

		ProcessMultiCastLinkData processLinkData = new ProcessMultiCastLinkData();
		WifiNetworkInfo wifiNetworkInfo = new WifiNetworkInfo();
		wifiNetworkInfo.setIp(mWiFiAdmin.getWifiIPAdress());
		wifiNetworkInfo.setSecurity(mWiFiAdmin.getSecurity());
		wifiNetworkInfo.setSsid(mWiFiAdmin.getWifiSSID());
		wifiNetworkInfo.setPassword(textPass.getText().toString());
		wifiNetworkInfo.setPort(0x3516);
		wifiNetworkInfo.setDeviceName(this.strName);
		wifiNetworkInfo.setOnlineProto(ONLINE_MSG_BY_TCP);
		
		processLinkData.constructMultiCastLinkMessageToSend(wifiNetworkInfo);
		
		//创建线程侦听上线消息
		recieveOnlineMessage();
		//发送报文
		sendMessage(processLinkData);
	}
	
	public void constructOnlineMessage(){
		byte []onlineMessageArray = new byte[13];
		byte []ackMessageArray = new byte[9];
		ackMessageArray[0] = 'O';
		ackMessageArray[1] = 'K';
		ackMessageArray[2] = ':';
		onlineMessageArray[0] = 'o';
		onlineMessageArray[1] = 'n';
		onlineMessageArray[2] = 'l';
		onlineMessageArray[3] = 'i';
		onlineMessageArray[4] = 'n';
		onlineMessageArray[5] = 'e';
		onlineMessageArray[6] = ':';
		int []macArray = new int[6];
		char []ssidArray = SSID.toCharArray();
		for(int i = 0; i < 6; ++i){
			macArray[i] = charToInt(ssidArray[12+2*i])*16+charToInt(ssidArray[12+2*i+1]);
		}
		for(int i = 0; i <6; ++i)
		{
			onlineMessageArray[7+i] = (byte)macArray[i];
			ackMessageArray[3+i] = onlineMessageArray[7+i];
		}
		ackMessage = new String(ackMessageArray, 0, 9);
		onlineMessage = new String(onlineMessageArray,0,13);
		for (int i = 0; i < 9; ++i)
			Log.d(TAG,"ackMessageArray["+i+"]="+ackMessageArray[i]);
		for (int i = 0; i < 13; ++i)
			Log.d(TAG,"onlineMessageArray["+i+"]="+onlineMessageArray[i]);
		Log.d(TAG,"onlineMessage="+onlineMessage);
		Log.d(TAG,"ackMessage="+ackMessage);
	}
	
	public int sendMessage(ProcessMultiCastLinkData processLinkData){
		//启动线程发送组播消息
		mMessageSend = new MessageSend(DeviceActivity.this);
		mMessageSend.multiCastThread(processLinkData);
		return 0;
	}
	
	public void recieveOnlineMessage(){
		onlineReciever = new OnlineReciever(new OnlineReciever.onOnlineRecievedListener() {
	        @Override
	        public void onOnlineReceived(String message) {
	        	Log.d(TAG, "message Received="+message);
	        	Log.d(TAG, "onlineMessage="+onlineMessage);
	        	if(onlineMessage.equals(message))
	        	{
	                Message msg = handler.obtainMessage();
	                msg.what = MSG_ONLINE_RECEIVED;
	                handler.sendMessage(msg);
	        	}
	        }
	    });
		onlineReciever.start();
	}
	
	public int charToInt(char input){
		int ret = 0;
		if('A' <= input){
			ret = input - 'A' + 10;
		}else{
			ret = input -'0';
		}
		return ret;
	}
	private final Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            switch(msg.what){
            case MSG_ONLINE_RECEIVED:
            	onlineRecieveTime = System.currentTimeMillis();
                long castTime = onlineRecieveTime-buttonPressTime;
            	//停止接收上线消息
            	onlineReciever.stop();
            	//停止广播
            	mMessageSend.stopMultiCast();
            	multicastTimer.cancel();
            	buttonConnectMulti.setText("连接成功");
            	errorHint.setText("恭喜！可以快乐的玩耍了！\n"+"耗时"+castTime+"ms!\n"+"超过了全国99.9%的用户！\n"+"buttonPressTime="+buttonPressTime+" onlineRecieveTime="+onlineRecieveTime);
            	break;
            case MSG_MULTICAST_TIMEOUT:
            	//停止接收上线消息
            	onlineReciever.stop();
            	//停止广播
            	mMessageSend.stopMultiCast();
            	buttonConnectMulti.setText("组播模式关联");
            	buttonConnectMulti.setClickable(true);
            	buttonConnectAP.setClickable(true);
            	errorHint.setText("连接失败，请确认以下内容：\n"+"1. 手机已关联家中WiFi；\n"+"2. 输入的密码正确；\n"+"3. 家中路由器工作在2.4G模式下。\n"+"若重试后仍然失败，请尝试点击AP模式关联");
            	break;
            	//AP收到单播消息
            case MSG_AP_RECEIVED_ACK:
            	Log.d(TAG,"MSG_AP_RECEIVED_ACK recieved");
            	//重新关联路由器AP
            	connectHomeAP();
            	udpPort = msg.arg1;
        		//创建UDP连接
        		BroadcastListenThread broadcastListenThread = new BroadcastListenThread();
        		broadcastListenThread.start(); 
            	break;
            case MSG_CONNECTED_APMODE:
            	APModeTimer.cancel();
            	long connectedTime = System.currentTimeMillis();
                long castTime2 = connectedTime-APModeStartTime;
            	buttonConnectAP.setText("连接成功");
            	errorHint.setText("恭喜！可以快乐的玩耍了！\n"+"耗时"+castTime2+"ms!\n"+"超过了全国99.9%的用户！\n"+"APModeStartTime="+APModeStartTime+" connectedTime="+connectedTime);
            	break;
            case MSG_APMODE_TIMEOUT:
            	//重新关联路由器AP
        		if ((null != mWiFiAdmin)&&(null != SSID))
        			mWiFiAdmin.forgetWifi(SSID);
            	connectHomeAP();
            	buttonConnectAP.setText("AP模式关联");
            	buttonConnectAP.setClickable(true);
            	buttonConnectMulti.setClickable(true);
            	errorHint.setText("连接失败，请确认以下内容，并重试：\n"+"1. 手机已关联家中WiFi；\n"+"2. 输入的密码正确；\n"+"3. 家中路由器工作在2.4G模式下。");
            	break;
            default:
            	break;
            }
        }
	};
	
	public void connectHomeAP(){
		
		//断开设备AP，关联路由器AP
		Log.d(TAG,"connect home wifi");
		mWiFiAdmin.enableNetWork(homeWifiID);
		//确认wifi已经关联上
		while(!mWiFiAdmin.isWifiConnected())
		{
			Log.d(TAG,"Wifi not connected");
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
	class BroadcastListenThread extends Thread{
		public void run(){
			while(isBroadcastListening){
				try {
					Log.d(TAG,"udpPort="+udpPort);
			        // 创建接收方的套接字,并制定端口号
			        DatagramSocket getSocket = new DatagramSocket(udpPort);
			        // 确定数据报接受的数据的数组大小  
			        byte[] buf = new byte[1024];  
			        // 创建接受类型的数据报，数据将存储在buf中  
			        DatagramPacket getPacket = new DatagramPacket(buf, buf.length);  
			        // 通过套接字接收数据  
			        getSocket.receive(getPacket);  
			        // 解析发送方传递的消息
			        String getMes = new String(buf, 0, getPacket.getLength());
			        Log.d(TAG,"recived udp msg "+getMes);
		        	if(getMes.equals(onlineMessage))
		        	{
		        		Log.d(TAG,"onlineMessage recieved ");
		        		isBroadcastListening = false;
		                Message msg = handler.obtainMessage();
		                msg.what = MSG_CONNECTED_APMODE;
		                handler.sendMessage(msg);
		        	}
		        	if ((null != getSocket)&&(!getSocket.isClosed()))
		        		getSocket.close();
				} 
				catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
	}


	
	class TCPConnectThread extends Thread{
		private ProcessAPLinkData processAPLinkData;

		public TCPConnectThread(ProcessAPLinkData processAPLinkData)
		{
			this.processAPLinkData = processAPLinkData;
		}
		
		public void run(){
			//确认WiFi已经关联上
			while(!mWiFiAdmin.isWifiConnected())
			{
				Log.d(TAG,"Wifi not connected");
				try {
					counterTime++;
					if (counterTime >= 20)
					{
						mWiFiAdmin.reconnect();
						counterTime = 0;
					}
					Thread.sleep(100);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			Log.d(TAG,"Wifi connected");
			if (0 == connect()){
				sendMessage(processAPLinkData);
			}
		}
	}
	
	public int connect(){
		try {
			//如果已经连接上了，就不再执行连接程序
			if (null == TCPSocket) {
				//用InetAddress方法获取ip地址
				InetAddress ipAddress = InetAddress.getByName("192.168.43.1");
				Log.e(TAG, "getHostAddress="+ipAddress.getHostAddress());
				Log.e(TAG, "ipAddress="+ipAddress);
				Log.d(TAG,"new Socket start");
				TCPSocket = new Socket(ipAddress, 5000);
				outputStream = TCPSocket.getOutputStream();
				TCPSocket.getInputStream();
				Log.d(TAG,"new Socket end");
			}
			if (null == TCPSocket){
                Log.d(TAG, "connet fail. socket == null");
                return -1;
			}
			return 0;
		} catch (UnknownHostException e1) {
			Log.e(TAG,"UnknownHostException error");
			e1.printStackTrace();
        	return -1;
		} catch (IOException e1) {
			Log.e(TAG,"IOException error");
			e1.printStackTrace();
			return -1;
		}
	}

	public void sendMessage(ProcessAPLinkData processAPLinkData){
        try {
            if(TCPSocket==null || !TCPSocket.isConnected()){
                Log.d(TAG, "SOCKET ERROR");
                return;
            }

			processAPLinkData.startSendAPLinkData(outputStream);

			//确认wifi已经去关联
			while(mWiFiAdmin.isWifiConnected())
			{
				Log.d(TAG,"Wifi connected");
				try {
					Thread.sleep(50);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			mWiFiAdmin.forgetWifi(SSID);
			Log.d(TAG,"Wifi disconnect");
            if(TCPSocket!=null && !TCPSocket.isClosed()){
            	TCPSocket.close();
            	TCPSocket=null;
            }
			Message msg = handler.obtainMessage();
		    msg.what = MSG_AP_RECEIVED_ACK;
		    msg.arg1 = ONLINE_PORT_BY_UDP;//udp port
		    handler.sendMessage(msg);
        } catch (IOException e) {
            e.printStackTrace();
        }
	}
	
	public WifiConfiguration constructWifiConfig(ProcessAPLinkData processAPLinkData,String ssid){
		String passwordString;

		passwordString = processAPLinkData.generateWifiPasswordBySSID(ssid);
		
		WifiConfiguration mWifiConfig = new WifiConfiguration();
		Log.d(TAG, "ssid="+ssid);
		Log.d(TAG, "passwordString="+passwordString);
		mWifiConfig = mWiFiAdmin.createWifiInfo(ssid, passwordString, 3);
		//mWifiConfig = mWiFiAdmin.createWifiInfo(SSID, null, 1);
		return mWifiConfig;
	}
}
