#ifndef SIGN_H
#define SIGN_H

#include <jni.h>
#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "polyvec.h"
#include "poly.h"

#define UNUSED(x) (void)(x)

#define challenge DILITHIUM_NAMESPACE(challenge)
void challenge(poly *c, const uint8_t seed[SEEDBYTES]);

JNIEXPORT jint JNICALL Java_com_beechat_network_Dilithium_crypto_1sign_1keypair
    (JNIEnv *env, jobject thisObject, jbyteArray pk, jbyteArray sk);

#define crypto_sign_signature DILITHIUM_NAMESPACE(signature)
int crypto_sign_signature(uint8_t *sig, size_t *siglen,
                          const uint8_t *m, size_t mlen,
                          const uint8_t *sk);

JNIEXPORT jint JNICALL Java_com_beechat_network_Dilithium_crypto_1sign
    (JNIEnv *env, jobject thisObject, jbyteArray _sm, jbyteArray _m, jlong _mlen, jbyteArray _sk);

#define crypto_sign_verify DILITHIUM_NAMESPACE(verify)
int crypto_sign_verify(const uint8_t *sig, size_t siglen,
                       const uint8_t *m, size_t mlen,
                       const uint8_t *pk);

JNIEXPORT jint JNICALL Java_com_beechat_network_Dilithium_crypto_1sign_1open
    (JNIEnv *env, jobject thisObject, jbyteArray _m, jbyteArray _sm, jlong _smlen, jbyteArray _pk);

#endif
