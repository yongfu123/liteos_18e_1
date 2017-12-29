package com.hisilicon.hisilink;

import java.util.List;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo.State;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiConfiguration.KeyMgmt;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

public class WiFiAdmin {
    static final int SECURITY_NONE = 0;  
    static final int SECURITY_WEP = 1;  
    static final int SECURITY_PSK = 2;  
    static final int SECURITY_EAP = 3; 
    static final int SECURITY_ERR = 4; 
	//定义一个WifiManager对象
	private WifiManager mWifiManager;
	//定义一个WifiInfo对象
	private WifiInfo mWifiInfo;
	//扫描出的网络连接列表
    private List<ScanResult> mWifiList;
    private List<WifiConfiguration> mWifiConfigurations;
    private ConnectivityManager mConnectivityManager;
    private static final String TAG = "WifiAdmin";
    public WiFiAdmin(Context context){
        //取得WifiManager对象
        mWifiManager=(WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        //取得WifiInfo对象  
        mWifiInfo=mWifiManager.getConnectionInfo();
        //得到配置好的网络连接  
        mWifiConfigurations=mWifiManager.getConfiguredNetworks();
        mConnectivityManager = (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);
    }
    
    public void startScan(){
        mWifiManager.startScan();
        //得到扫描结果
        mWifiList=mWifiManager.getScanResults();
    }
    
    public boolean isWifiEnabled(){
    	return mWifiManager.isWifiEnabled();
    }
    
    //得到网络列表  
    public List<ScanResult> getWifiList(){  
        return mWifiList;
    }
    
    //得到手机关联的wifi的SSID
    public String getWifiSSID(){
    	mWifiInfo=mWifiManager.getConnectionInfo();
    	String str = (mWifiInfo==null)?"NULL":mWifiInfo.getSSID();
    	//去掉ssid前后的双引号
        str = str.substring(1,str.length()-1);
        return str;
    }
    
    //得到手机关联的wifi的ip地址
    public String getWifiIPAdress(){
    	int ipAddress = (mWifiInfo==null)?0:mWifiInfo.getIpAddress();
        return String.format("%c%c%c%c",
                (ipAddress & 0xff), (ipAddress >> 8 & 0xff),
                (ipAddress >> 16 & 0xff), (ipAddress >> 24 & 0xff));
    }

    //得到关联的wifi的加密方式
    public int getSecurity() {
    	for (WifiConfiguration mWifiConfiguration : mWifiConfigurations) {
    		//当前连接的SSID
    		String mCurrentSSID = getWifiSSID();
    		
    		//配置过的SSID
            String mConfigSSid = mWifiConfiguration.SSID;
            //去掉ssid前后的双引号
            mConfigSSid = mConfigSSid.substring(1,mConfigSSid.length()-1);
            
          //比较networkId，防止配置网络保存相同的SSID
            if (mCurrentSSID.equals(mConfigSSid)&&mWifiInfo.getNetworkId()==mWifiConfiguration.networkId) {
                if (mWifiConfiguration.allowedKeyManagement.get(KeyMgmt.WPA_PSK)) {
                    return SECURITY_PSK;  
                }
                if (mWifiConfiguration.allowedKeyManagement.get(KeyMgmt.WPA_EAP) || mWifiConfiguration.allowedKeyManagement.get(KeyMgmt.IEEE8021X)) {  
                    return SECURITY_EAP;  
                }
                return (mWifiConfiguration.wepKeys[0] != null) ? SECURITY_WEP : SECURITY_NONE;  
            }
    	}
    	//error: 配置SSID列表中未找到当前网络
		return SECURITY_ERR;
    }
    
    //添加一个网络并连接  
    public void addNetWork(WifiConfiguration configuration){  
        int wcgId=mWifiManager.addNetwork(configuration);  
        mWifiManager.enableNetwork(wcgId, true);
        mWifiManager.reconnect();
        mWifiManager.saveConfiguration();
    }
    //重新连接  
    public void reconnect(){  
    	mWifiManager.reconnect();
    }
    //enable 网络
    public void enableNetWork(int wcgId){
        mWifiManager.enableNetwork(wcgId, true);
    }

    public boolean isWifiConnected()
    {
    	State wifi = mConnectivityManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI).getState();
    	if(wifi == State.CONNECTED)
            return true;
    	else
    		return false;
    }
    
    //得到连接的ID  
    public int getNetWorkId(){  
        return (mWifiInfo==null)?-1:mWifiInfo.getNetworkId();  
    }
    
    //断开指定ID的网络  
    public void disConnectionWifi(int netId){
        mWifiManager.disableNetwork(netId);
        mWifiManager.disconnect();
    }

    private WifiConfiguration IsExsits(String SSID) {
        List<WifiConfiguration> existingConfigs = mWifiManager.getConfiguredNetworks();
        if (existingConfigs == null)
            return null;
        for (WifiConfiguration existingConfig : existingConfigs) {
            if (existingConfig.SSID!=null && existingConfig.SSID.equals("\"" + SSID + "\"")) {
                return existingConfig;
            }
        }
        return null;
    }

    //断开并删除指定ID的网络  
    public void forgetWifi(String SSID){
        WifiConfiguration tempConfig = this.IsExsits(SSID);
        if (tempConfig != null) {
        	Log.e(TAG,"tempConfig.networkId="+tempConfig.networkId);
            mWifiManager.removeNetwork(tempConfig.networkId);
            mWifiManager.saveConfiguration();
        }
    }
    
    //根据SSID 密码 加密方式配置网络
    public WifiConfiguration createWifiInfo(String SSID, String Password, int Type){
    	WifiConfiguration config = new WifiConfiguration();
    	config.allowedAuthAlgorithms.clear();
        config.allowedGroupCiphers.clear();
        config.allowedKeyManagement.clear();
        config.allowedPairwiseCiphers.clear();
        config.allowedProtocols.clear();
        config.SSID = "\"" + SSID + "\"";

        if(Type == 1) //WIFICIPHER_NOPASS
        {
        	Log.e(TAG,"set open wifi");
        	config.hiddenSSID = true;
        	config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
        }

        if(Type == 2) //WIFICIPHER_WEP
        {
        	config.hiddenSSID = true;
        	config.wepKeys[0]= "\""+Password+"\"";
        	config.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.SHARED);
        	config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
        	config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
        	config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP40);
        	config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP104);
        	config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
        	config.wepTxKeyIndex = 0;
        }
        if(Type == 3) //WIFICIPHER_WPA
        {
        	config.preSharedKey = "\""+Password+"\"";     
        	config.hiddenSSID = true;       
        	config.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.OPEN);
        	config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
        	config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
        	config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
        	config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
        	config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
        	config.status = WifiConfiguration.Status.ENABLED;
        }
        return config;
    }     
}
