package com.hisilicon.hisilink;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.net.wifi.ScanResult;
import android.os.Bundle;
import android.provider.Settings;
import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

public class MainActivity extends Activity {
	private static final String TAG = "MainActivity";
	private WiFiAdmin mWiFiAdmin;
	private ListView lvDevices = null;
	private Map<String, String> companyIdMap = null;
	private Map<String, String> deviceTypeMap = null;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		ActionBar actionBar = getActionBar();  
		actionBar.hide();
		mWiFiAdmin = new WiFiAdmin(MainActivity.this);
		if ((!mWiFiAdmin.isWifiEnabled()) || (0 > mWiFiAdmin.getNetWorkId())){
			warningDialog();
		}
		initCompanyIdMap();
		initDeviceTypeMap();
	}
	
	public void initCompanyIdMap()
	{	
		//厂商ID map表初始化
		companyIdMap= new HashMap<String, String>();
		companyIdMap.put("001", "海思");
		companyIdMap.put("002", "华为");
		companyIdMap.put("003", "hi1131s");
		companyIdMap.put("004", "小米");
		//to be continue
	}
	
	public void initDeviceTypeMap()
	{
		//设备类型 map表初始化
		deviceTypeMap= new HashMap<String, String>();
		deviceTypeMap.put("001", "随身拍");
		deviceTypeMap.put("002", "智能门铃");
		deviceTypeMap.put("003", "运动DV");
		deviceTypeMap.put("004", "无人机");
		//to be continue
	}
	
	public void warningDialog(){
		// Android对话框
		AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);//创建对话框
		builder.setTitle("请打开Wifi，并关联家庭网络"); //设置标题
		builder.setMessage("是否前往设置？"); //设置内容
		builder.setIcon(android.R.drawable.ic_menu_info_details); //设置图标
		builder.setCancelable(false);
		builder.setPositiveButton("是", new DialogInterface.OnClickListener() {
		    @Override
		    public void onClick(DialogInterface dialogInterface, int i) {
		    	startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
		    }
		});//设置按钮及功能实现
		builder.setNegativeButton("否", new DialogInterface.OnClickListener() {
		    @Override
		    public void onClick(DialogInterface dialogInterface, int i) {
		    	finish();
		    }
		});//设置按钮及功能实现
		builder.show(); //显示对话框
	}
	
	//寻找格式符合特殊SSID要求的设备，找到返回解析后的设备名，否则返回null
	//Hisi_XYYYZZZUUUUUUUUUUUU
	public String findDeviceBySsid(String ssid){
		String retStr = null;
		if((ssid.length()>=24) && (ssid.startsWith("Hisi_"))){
			//X代表版本号，应该介于1到9之间
			if((ssid.charAt(5) < '1')||(ssid.charAt(5) > '9'))
				return null;
			//YYY代表制造商ID，目前002代表华为海思，其他制造商待追加。
			if (companyIdMap.containsKey(ssid.substring(6, 9))){
				Log.d(TAG,"companyIdMap="+companyIdMap.get(ssid.substring(6, 9)));
				retStr = companyIdMap.get(ssid.substring(6, 9));
			}
			else{
				return null;
			}
			
			//ZZZ代表设备类型
			if (deviceTypeMap.containsKey(ssid.substring(9, 12))){
				Log.d(TAG,"deviceTypeMap="+deviceTypeMap.get(ssid.substring(9, 12)));
				retStr += deviceTypeMap.get(ssid.substring(9, 12));
			}
			else{
				return null;
			}
			
			//MAC地址后四位
			retStr += ssid.substring(20, 24);
		}
		return retStr;
	}
	
	public void onClick_Event(View view) {
		List<ScanResult> mWiFiList;
		final List<String> deviceList = new ArrayList<String>();
		final List<String> ssidList = new ArrayList<String>();
		
		lvDevices = (ListView)findViewById(R.id.deviceslist);

		try {
			mWiFiAdmin.startScan();
			Thread.sleep(50);
			mWiFiList = mWiFiAdmin.getWifiList();
			String []str = new String[mWiFiList.size()];
			
			for(int i = 0; i < mWiFiList.size(); ++i){
				str[i] = findDeviceBySsid(mWiFiList.get(i).SSID);
				if((null != str[i])&&(!deviceList.contains(str[i]))){
					deviceList.add(str[i]);
					ssidList.add(mWiFiList.get(i).SSID);
				}
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		if(deviceList.size() != 0){
			lvDevices.setAdapter(new ArrayAdapter<String>(this,
	                android.R.layout.simple_list_item_1, deviceList));
			lvDevices.setOnItemClickListener(new OnItemClickListener(){
				public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
	                    long arg3) {
					Intent intent = new Intent(MainActivity.this, DeviceActivity.class);
					Bundle bundle=new Bundle();
					bundle.putString("devicename", deviceList.get(arg2));
					bundle.putString("SSID", ssidList.get(arg2));
					intent.putExtras(bundle);
                    startActivity(intent);
				}
			});
		}

		if(0 == deviceList.size())
		{
			setContentView(R.layout.activity_main);
			Toast toast = Toast.makeText(getApplicationContext(), "没有检索到设备，请确认", Toast.LENGTH_SHORT);
			toast.show(); 
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
