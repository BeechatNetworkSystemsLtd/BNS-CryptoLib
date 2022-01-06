#include <jni.h>
#include <stddef.h>
#include <stdint.h>

#include "blake3.h"

#ifndef _Included_com_beechat_network_Blake3
#define _Included_com_beechat_network_Blake3
#ifdef __cplusplus
extern "C" {
#endif

void java_to_c_object(blake3_hasher *str, JNIEnv *env, jobject obj);
void c_to_java_object(blake3_hasher *str, JNIEnv *env, jobject obj);
/*
 * Class:     network_beechat_Blake3
 * Method:    init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_beechat_network_Blake3_init
  (JNIEnv *, jobject);

/*
 * Class:     network_beechat_Blake3
 * Method:    update_native
 * Signature: ([BI)V
 */
JNIEXPORT void JNICALL Java_com_beechat_network_Blake3_update_1native
  (JNIEnv *, jobject, jbyteArray, jint);

/*
* Class:     network_beechat_Blake3
* Method:    update_native
* Signature: ([BI)V
*/
JNIEXPORT void JNICALL Java_com_beechat_network_Blake3_update_1critical_1native_1raw
  (JNIEnv *env, jclass obj, jbyteArray _context, jint _len
  , jintArray key
  , jint cv_stack_len
  , jbyteArray cv_stack
  , jintArray cv
  , jlong chunk_counter
  , jbyteArray buf
  , jbyte buf_len
  , jbyte blocks_compressed
  , jbyte flags
  , jlongArray outputs);

/*
 * Class:     network_beechat_Blake3
 * Method:    update_native
 * Signature: ([BI)V
 */
JNIEXPORT void JNICALL
JavaCritical_com_beechat_network_Blake3_update_1critical_1native_1raw(
    jbyte *_context
  , jint _len
  , jint *key
  , jint cv_stack_len
  , jbyte *cv_stack
  , jint *cv
  , jlong chunk_counter
  , jbyte *buf
  , jbyte buf_len
  , jbyte blocks_compressed
  , jbyte flags
  , jlong *outputs
);

/*
 * Class:     network_beechat_Blake3
 * Method:    finalize_seek_native
 * Signature: (J[BI)V
 */
JNIEXPORT void JNICALL Java_com_beechat_network_Blake3_finalize_1seek_1native
  (JNIEnv *, jobject, jlong, jbyteArray, jint);


JNIEXPORT jint JNICALL Java_com_beechat_network_Blake3_init_1keyed(
    JNIEnv *env
  , jobject obj
  , jbyteArray key
);


JNIEXPORT jint JNICALL Java_com_beechat_network_Blake3_init_1derive_1key(
    JNIEnv *env
  , jobject obj
  , jcharArray context
  , jint len
);


#ifdef __cplusplus
}
#endif
#endif
