package com.hisilicon.hisilink;

import com.huawei.hi1131s.hisilink.api.ProcessMultiCastLinkData;

import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;

public class MessageSend {
	private static final String TAG = "MessageSend";
	private Handler mHandler; 
	private ProcessMultiCastLinkData processLinkData;
	
    public MessageSend(Context context){
    }

    public void multiCastThread(ProcessMultiCastLinkData processLinkData){
    	this.processLinkData = processLinkData;
        //通过Handler启动线程    
    	HandlerThread handlerThread = new HandlerThread("MultiSocketA");
        handlerThread.start(); 
        mHandler =  new Handler(handlerThread.getLooper());  
        mHandler.post(mRunnable);
        
        
    }
    public void stopMultiCast(){
    	processLinkData.stopSendMultiBroadcast(false);
    }
    private Runnable mRunnable = new Runnable() {
        public void run() {  
            Log.v(TAG, "run...");  
            try {
            	processLinkData.startSendMultiBroadcast(true);
            } catch (Exception e) {  
                e.printStackTrace();  
            }  
        }
    };
}
