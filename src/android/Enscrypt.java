package com.sqrlauth.enscrypt;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

public class Enscrypt implements Runnable {
	protected native int enscrypt_ms( byte[] buf, String passwd, byte[] salt, int millis );
	protected native int enscrypt( byte[] buf, String passwd, byte[] salt, int iterations );
	protected native double get_real_time();
	
	protected Handler handler;
	protected Enscrypt.EnscryptListener myListener;
	protected String password = null;
	protected byte[] salt = null;
	protected byte[] result = new byte[32];
	protected int iterations = 0;
	protected int duration = 0;
	protected boolean cancelled = false;

	public interface EnscryptListener {
		public void onEnscryptFinished( byte[] result );
		public void onEnscryptProgress(int p);
		public void onEnscryptError( String msg );
	}
	
	static { System.loadLibrary( "enscrypt" ); }
	
	public Enscrypt(EnscryptListener listener) {
		this.myListener = listener;
		this.handler = new Handler() {
			@Override
			public void handleMessage( Message msg ) {
				if( msg.what == 100 ) {
					myListener.onEnscryptProgress(100);
					if( !cancelled ) myListener.onEnscryptFinished(result);
				} else if( msg.what == -2 ) {
					myListener.onEnscryptError(new String(msg.getData().getCharArray("msg")));
				} else {
					myListener.onEnscryptProgress(msg.what);
				}
			}
		};
	}
	
	public void clean() {
		this.salt = null;
		this.result = null;
		this.password = null;
		this.iterations = 0;
		this.duration = 0;
		this.cancelled = false;
	}

	public void doIterations( int iterations ) {
		if( iterations < 1 ) iterations = 1;
		this.iterations = iterations;
		this.duration = 0;
		this.cancelled = false;
		new Thread( this ).start();
	}
	
	public void doMilliseconds( int millis ) {
		if( millis < 1 ) millis = 1;
		this.duration = millis;
		this.iterations = 0;
		this.cancelled = false;
		new Thread( this ).start();
	}
	
	public void cancel() {
		this.cancelled = true;
	}
	
	private void enscryptFatalError( char[] msg ) {
		Bundle b = new Bundle();
		b.putCharArray("msg", msg);
		Message message = new Message();
		message.setData(b);
		message.what = -2;
		this.handler.sendMessage(message);
	}
	
	@Override
	public void run() {
		if( iterations > 0 ) {
			int retVal = this.enscrypt( this.result, this.password, this.salt, this.iterations);
			this.duration = retVal;
		} else {
			int retVal = this.enscrypt_ms( this.result, this.password, this.salt, this.duration );
			this.iterations = retVal;
		}
	}
	
	@Override
	public String toString() {
		return this.result.toString();
	}

	public int sendProgressMessage( int p ) {
		this.handler.sendEmptyMessage( p );
		if( cancelled ) return 0;
		return 1;
	}
	
	public String getPassword() {
		return password;
	}
	public void setPassword(String password) {
		this.password = password;
	}
	public void clearPassword() {
		this.password = null;
	}
	public byte[] getSalt() {
		return salt;
	}
	public void setSalt(byte[] salt) {
		this.salt = salt;
	}
	public int getIterations() {
		return iterations;
	}
	public void setIterations(int iterations) {
		this.iterations = iterations;
	}
	public int getDuration() {
		return duration;
	}
	public void setDuration(int duration) {
		this.duration = duration;
	}
	public byte[] getResult() {
		return this.result;
	}
	public void setResult(byte[] result) {
		this.result = result;
	}
	public boolean getCancelled() {
		return this.cancelled;
	}
}
