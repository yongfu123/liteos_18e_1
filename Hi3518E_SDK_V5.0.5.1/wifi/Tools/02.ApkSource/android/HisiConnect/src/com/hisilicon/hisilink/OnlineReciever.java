package com.hisilicon.hisilink;

import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;

import android.util.Log;

public class OnlineReciever {
	private static final String TAG = "OnlineReciever";
	private Thread mServerThread;
	private ServerSocket mServerSocket = null;
	private static final int SERVER_PORT = 13590;
	private volatile boolean isListening = true;
	
	public interface onOnlineRecievedListener {
        void onOnlineReceived(String recieveMessage);
    }
	private onOnlineRecievedListener mListener;
    public OnlineReciever(onOnlineRecievedListener listener){
        mListener = listener;
    }
    public void start() {

        isListening = true;

        mServerThread = new ServerThread();
        mServerThread.setName("Online Message Receive Thread");
        mServerThread.start();
    }
    public void stop() {

        isListening = false;

        try{
            if( null!= mServerSocket && !mServerSocket.isClosed() ){
                mServerSocket.close();
            }
            if( null!=mServerThread && mServerThread.isAlive() ){
                mServerThread.interrupt();
            }
        } catch (Exception e){
            e.printStackTrace();
        }

        mServerSocket = null;
    }
    private class ServerThread extends Thread {
    	@Override
        public void run(){
    		do{
	            Socket socket = null;
	            try{
	                    if( null== mServerSocket || mServerSocket.isClosed() ){
	                    	mServerSocket = new ServerSocket(SERVER_PORT);
	                        mServerSocket.setReuseAddress(true);
	                    }

	                    byte[] buffer = new byte[1000];

	                    Log.d(TAG, "mServerSocket.accept in");
	                    socket = mServerSocket.accept();
	                    Log.d(TAG, "mServerSocket.accept out");

	                    socket.setSoTimeout(45000);
                        int length = 0;
	                    InputStream stream = socket.getInputStream();
	                    do{
	                    	int temp = stream.read(buffer, length, buffer.length - length);
	                        if(temp<=0){
	                        	Log.d(TAG, "TCP Received over");
	                            break; //£¨¶Ï¿ª£©
	                        }
	                        length+=temp;
	                    }while( true );
	                    
	                    stream.close();
	                    socket.close();
	                    socket = null;
	                    
	                    if (length > 0){
	                		for (int i = 0; i < length; ++i)
	                			Log.d(TAG,"recived buffer["+i+"]="+buffer[i]);
	                		Log.d(TAG,"recived buffer length="+length);
	                		String getMes = new String(buffer, 0, length);
	                    	mListener.onOnlineReceived(getMes);
	                    }
	            }catch(Exception e){
	                e.printStackTrace();
	                if(socket !=null && !socket.isClosed()){
	                    try {
	                        socket.close();
	                    } catch (IOException e1) {
	                        e1.printStackTrace();
	                    }
	                }
	            }
	        }while(isListening);
    	}
    }
}
