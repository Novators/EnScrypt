package com.sqrlauth.enscrypt;

import android.os.Handler;
import android.os.Message;

public class Enscrypt implements Runnable {
	protected native int enscrypt_ms( byte[] buf, String passwd, byte[] salt, int millis );
	protected native int enscrypt( byte[] buf, String passwd, byte[] salt, int iterations );
	protected native double get_real_time();
	
	protected Handler handler;
	protected Enscrypt.EnscryptListener listener;
	protected String password = null;
	protected byte[] salt = null;
	protected byte[] result = new byte[32];
	protected int iterations = 0;
	protected int duration = 0;

	public interface EnscryptListener {
		public void enscryptFinished( byte[] result );
		public void enscryptProgress(int p);
	}
	
	static { System.loadLibrary( "enscrypt" ); }
	
	public Enscrypt(EnscryptListener listener) {
		this.listener = listener;
		this.handler = new Handler() {
			@Override
			public void handleMessage( Message msg ) {
				if( msg.what == -1 ) {
					handleFinished();
				} else {
					handleProgress( msg.what );
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
	}

	private void handleFinished() {
		listener.enscryptFinished(result);
	}
	
	private void handleProgress( int p ) {
		listener.enscryptProgress(p);
	}
	
	public void doIterations( int iterations ) {
		if( iterations < 1 ) iterations = 1;
		this.iterations = iterations;
		this.duration = 0;
		new Thread( this ).start();
	}
	
	public void doMilliseconds( int millis ) {
		if( millis < 1 ) millis = 1;
		this.duration = millis;
		this.iterations = 0;
		new Thread( this ).start();
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
		this.handler.sendEmptyMessage(-1);
	}
	
	@Override
	public String toString() {
		return this.result.toString();
	}

	public void sendProgressMessage( int p ) {
		this.handler.sendEmptyMessage( p );
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

}
