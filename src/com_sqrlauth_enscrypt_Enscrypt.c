#include <jni.h>
#include <stdint.h>
#include <string.h>
#include "com_sqrlauth_enscrypt_Enscrypt.h"
#include "enscrypt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct progress_data {
	JNIEnv *env;
	jobject obj;
	jmethodID mid;
} progress_data;

static progress_data fatal_error_data;

static int progressFn( int p, void *cb_data ) {
	progress_data *d = (progress_data*)cb_data;
	return (*(d->env))->CallIntMethod( d->env, d->obj, d->mid, p );
}

static void fatal_error_function( const char *data ) {
	JNIEnv *env = fatal_error_data.env;
	size_t len = strlen(data);
	jchar jca[len];
	int i;
	for( i = 0; i < len; i++ ) {
		jca[i] = (jchar)data[i];
	}
	jcharArray charArr = (*env)->NewCharArray(fatal_error_data.env, len + 1 );
	(*(fatal_error_data.env))->SetCharArrayRegion( fatal_error_data.env, charArr, 0, len, (jchar*)jca );
	if( fatal_error_data.mid ) {
		(*(fatal_error_data.env))->CallVoidMethod(
			fatal_error_data.env,
			fatal_error_data.obj,
			fatal_error_data.mid,
			charArr
		);
	}
}

static void power_on( JNIEnv *env, jobject obj ) {
	fatal_error_data.env = env;
	fatal_error_data.obj = obj;
	fatal_error_data.mid = (*env)->GetMethodID( env, (*env)->GetObjectClass( env, obj ), "enscryptFatalError", "([C)V" );
	enscrypt_set_fatal_error( fatal_error_function );
}

/*
 * Class:     com_sqrlauth_enscrypt_Enscrypt
 * Method:    enscrypt_ms
 * Signature: ([BLjava/lang/String;[BI)I
 */
JNIEXPORT jint JNICALL Java_com_sqrlauth_enscrypt_Enscrypt_enscrypt_1ms
  (JNIEnv *env, jobject jobj, jbyteArray buf, jstring password, jbyteArray salt, jint millis)
{
	jsize b_ln = 0, s_ln = 0, p_ln = 0;
	jint retVal;
	uint8_t *mBuf;
	uint8_t *mSalt = NULL;
	const char *mPass;
	progress_data d;
	
	if( millis < 1 ) return -1;
	if( buf ) {
		b_ln = (*env)->GetArrayLength( env, buf );
		if( b_ln != 32 ) return -1;
	} else {
		return -1;
	}
	if( salt ) {
		s_ln = (*env)->GetArrayLength( env, salt );
		if( s_ln != 32 ) return -1;
	}
	d.env = env;
	d.obj = jobj;
	d.mid = (*env)->GetMethodID( env, (*env)->GetObjectClass( env, jobj ), "sendProgressMessage", "(I)I" );
	if( d.mid == 0 ) return -1;
	
	power_on( env, jobj );
	
	mBuf = (uint8_t*)((*env)->GetByteArrayElements(env, buf, NULL));
	if( s_ln ) {
		mSalt = (uint8_t*)((*env)->GetByteArrayElements(env, salt, NULL));
	}
	if( password ) {
		p_ln = (*env)->GetStringUTFLength(env, password);
		if( p_ln ) {
			mPass = (*env)->GetStringUTFChars(env, password, NULL);
		}
	}
	
	if( p_ln ) {
		retVal = enscrypt_ms(mBuf, mPass, mSalt, millis, progressFn, &d );
	} else {
		retVal = enscrypt_ms(mBuf, NULL, mSalt, millis, progressFn, &d );
	}
	
	(*env)->ReleaseByteArrayElements(env, buf, (jbyte*)mBuf, 0);
	if( mSalt ) {
		(*env)->ReleaseByteArrayElements(env, salt, (jbyte*)mSalt, JNI_ABORT);
	}
	if( p_ln ) {
		(*env)->ReleaseStringUTFChars(env, password, mPass);
	}
	return retVal;
}

/*
 * Class:     com_sqrlauth_enscrypt_Enscrypt
 * Method:    enscrypt
 * Signature: ([BLjava/lang/String;[BI)I
 */
JNIEXPORT jint JNICALL Java_com_sqrlauth_enscrypt_Enscrypt_enscrypt
  (JNIEnv *env, jobject jobj, jbyteArray buf, jstring password, jbyteArray salt, jint iterations)
{
	jsize b_ln = 0, s_ln = 0, p_ln = 0;
	jint retVal;
	uint8_t *mBuf;
	uint8_t *mSalt = NULL;
	const char *mPass;
	progress_data d;
	
	if( iterations < 1 ) return -6;
	if( buf ) {
		b_ln = (*env)->GetArrayLength( env, buf );
		if( b_ln != 32 ) return -2;
	} else {
		return -3;
	}
	if( salt ) {
		s_ln = (*env)->GetArrayLength( env, salt );
		if( s_ln != 32 ) return -4;
	}
	d.env = env;
	d.obj = jobj;
	d.mid = (*env)->GetMethodID( env, (*env)->GetObjectClass( env, jobj ), "sendProgressMessage", "(I)I" );
	if( d.mid == 0 ) return -5;
	
	power_on( env, jobj );
	
	mBuf = (uint8_t*)((*env)->GetByteArrayElements(env, buf, NULL));
	if( s_ln ) {
		mSalt = (uint8_t*)((*env)->GetByteArrayElements(env, salt, NULL));
	}
	if( password ) {
		p_ln = (*env)->GetStringUTFLength(env, password);
		if( p_ln ) {
			mPass = (*env)->GetStringUTFChars(env, password, NULL);
		}
	}
	
	if( p_ln ) {
		retVal = enscrypt(mBuf, mPass, mSalt, iterations, progressFn, &d );
	} else {
		retVal = enscrypt(mBuf, NULL, mSalt, iterations, progressFn, &d );
	}
	
	(*env)->ReleaseByteArrayElements(env, buf, (jbyte*)mBuf, 0);
	if( mSalt ) {
		(*env)->ReleaseByteArrayElements(env, salt, (jbyte*)mSalt, JNI_ABORT);
	}
	if( p_ln ) {
		(*env)->ReleaseStringUTFChars(env, password, mPass);
	}
	if( retVal < 1 ) return -11;
	return retVal;
}

/*
 * Class:     com_sqrlauth_enscrypt_Enscrypt
 * Method:    get_real_time
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_sqrlauth_enscrypt_Enscrypt_get_1real_1time
  (JNIEnv *env, jobject jobj)
{
	return enscrypt_get_real_time();
}

#ifdef __cplusplus
}
#endif
