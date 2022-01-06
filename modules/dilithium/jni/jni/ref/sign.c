#include <stdint.h>
#include <stdlib.h>
#include "params.h"
#include "ssign.h"
#include "packing.h"
#include "polyvec.h"
#include "poly.h"
#include "randombytes.h"
#include "symmetric.h"
#include "fips202.h"

/*************************************************
* Name:        crypto_sign_keypair
*
* Description: Generates public and private key.
*
* Arguments:   - uint8_t *pk: pointer to output public key (allocated
*                             array of CRYPTO_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key (allocated
*                             array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
JNIEXPORT jint JNICALL Java_com_beechat_network_Dilithium_crypto_1sign_1keypair
    (JNIEnv *env, jobject thisObject, jbyteArray _pk, jbyteArray _sk)
{
  uint8_t seedbuf[2*SEEDBYTES + CRHBYTES];
  uint8_t tr[SEEDBYTES];
  const uint8_t *rho, *rhoprime, *key;
  polyvecl mat[K];
  polyvecl s1, s1hat;
  polyveck s2, t1, t0;

  size_t i;
  uint8_t pk[CRYPTO_PUBLICKEYBYTES] = { 0 };
  uint8_t sk[CRYPTO_SECRETKEYBYTES] = { 0 };
  jbyte _pkj[CRYPTO_PUBLICKEYBYTES] = { 0 };
  jbyte _skj[CRYPTO_SECRETKEYBYTES] = { 0 };

  UNUSED(thisObject);

  /* Get randomness for rho, rhoprime and key */
  randombytes(seedbuf, SEEDBYTES);
  shake256(seedbuf, 2*SEEDBYTES + CRHBYTES, seedbuf, SEEDBYTES);
  rho = seedbuf;
  rhoprime = rho + SEEDBYTES;
  key = rhoprime + CRHBYTES;

  /* Expand matrix */
  polyvec_matrix_expand(mat, rho);

  /* Sample short vectors s1 and s2 */
  polyvecl_uniform_eta(&s1, rhoprime, 0);
  polyveck_uniform_eta(&s2, rhoprime, L);

  /* Matrix-vector multiplication */
  s1hat = s1;
  polyvecl_ntt(&s1hat);
  polyvec_matrix_pointwise_montgomery(&t1, mat, &s1hat);
  polyveck_reduce(&t1);
  polyveck_invntt_tomont(&t1);

  /* Add error vector s2 */
  polyveck_add(&t1, &t1, &s2);

  /* Extract t1 and write public key */
  polyveck_caddq(&t1);
  polyveck_power2round(&t1, &t0, &t1);
  pack_pk(pk, rho, &t1);

  /* Compute H(rho, t1) and write secret key */
  shake256(tr, SEEDBYTES, pk, CRYPTO_PUBLICKEYBYTES);
  pack_sk(sk, rho, tr, key, &t0, &s1, &s2);

  for (i = 0; i < CRYPTO_PUBLICKEYBYTES; i++)
  {
    _pkj[i] = pk[i];
  }
  for (i = 0; i < CRYPTO_SECRETKEYBYTES; i++)
  {
    _skj[i] = sk[i];
  }

  (*env)->SetByteArrayRegion(env, _pk, 0, CRYPTO_PUBLICKEYBYTES, _pkj);
  (*env)->SetByteArrayRegion(env, _sk, 0, CRYPTO_SECRETKEYBYTES, _skj);

  return 0;
}

/*************************************************
* Name:        crypto_sign_signature
*
* Description: Computes signature.
*
* Arguments:   - uint8_t *sig:   pointer to output signature (of length CRYPTO_BYTES)
*              - size_t *siglen: pointer to output length of signature
*              - uint8_t *m:     pointer to message to be signed
*              - size_t mlen:    length of message
*              - uint8_t *sk:    pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
int crypto_sign_signature(uint8_t *sig,
                          size_t *siglen,
                          const uint8_t *m,
                          size_t mlen,
                          const uint8_t *sk)
{
  unsigned int n;
  uint8_t seedbuf[3*SEEDBYTES + 2*CRHBYTES];
  uint8_t *rho, *tr, *key, *mu, *rhoprime;
  uint16_t nonce = 0;
  polyvecl mat[K], s1, y, z;
  polyveck t0, s2, w1, w0, h;
  poly cp;
  keccak_state state;

  rho = seedbuf;
  tr = rho + SEEDBYTES;
  key = tr + SEEDBYTES;
  mu = key + SEEDBYTES;
  rhoprime = mu + CRHBYTES;
  unpack_sk(rho, tr, key, &t0, &s1, &s2, sk);

  /* Compute CRH(tr, msg) */
  shake256_init(&state);
  shake256_absorb(&state, tr, SEEDBYTES);
  shake256_absorb(&state, m, mlen);
  shake256_finalize(&state);
  shake256_squeeze(mu, CRHBYTES, &state);

#ifdef DILITHIUM_RANDOMIZED_SIGNING
  randombytes(rhoprime, CRHBYTES);
#else
  shake256(rhoprime, CRHBYTES, key, SEEDBYTES + CRHBYTES);
#endif

  /* Expand matrix and transform vectors */
  polyvec_matrix_expand(mat, rho);
  polyvecl_ntt(&s1);
  polyveck_ntt(&s2);
  polyveck_ntt(&t0);

rej:
  /* Sample intermediate vector y */
  polyvecl_uniform_gamma1(&y, rhoprime, nonce++);

  /* Matrix-vector multiplication */
  z = y;
  polyvecl_ntt(&z);
  polyvec_matrix_pointwise_montgomery(&w1, mat, &z);
  polyveck_reduce(&w1);
  polyveck_invntt_tomont(&w1);

  /* Decompose w and call the random oracle */
  polyveck_caddq(&w1);
  polyveck_decompose(&w1, &w0, &w1);
  polyveck_pack_w1(sig, &w1);

  shake256_init(&state);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_absorb(&state, sig, K*POLYW1_PACKEDBYTES);
  shake256_finalize(&state);
  shake256_squeeze(sig, SEEDBYTES, &state);
  poly_challenge(&cp, sig);
  poly_ntt(&cp);

  /* Compute z, reject if it reveals secret */
  polyvecl_pointwise_poly_montgomery(&z, &cp, &s1);
  polyvecl_invntt_tomont(&z);
  polyvecl_add(&z, &z, &y);
  polyvecl_reduce(&z);
  if(polyvecl_chknorm(&z, GAMMA1 - BETA))
    goto rej;

  /* Check that subtracting cs2 does not change high bits of w and low bits
   * do not reveal secret information */
  polyveck_pointwise_poly_montgomery(&h, &cp, &s2);
  polyveck_invntt_tomont(&h);
  polyveck_sub(&w0, &w0, &h);
  polyveck_reduce(&w0);
  if(polyveck_chknorm(&w0, GAMMA2 - BETA))
    goto rej;

  /* Compute hints for w1 */
  polyveck_pointwise_poly_montgomery(&h, &cp, &t0);
  polyveck_invntt_tomont(&h);
  polyveck_reduce(&h);
  if(polyveck_chknorm(&h, GAMMA2))
    goto rej;

  polyveck_add(&w0, &w0, &h);
  n = polyveck_make_hint(&h, &w0, &w1);
  if(n > OMEGA)
    goto rej;

  /* Write signature */
  pack_sig(sig, sig, &z, &h);
  *siglen = CRYPTO_BYTES; return 0;
}

/*************************************************
* Name:        crypto_sign
*
* Description: Compute signed message.
*
* Arguments:   - uint8_t *sm: pointer to output signed message (allocated
*                             array with CRYPTO_BYTES + mlen bytes),
*                             can be equal to m
*              - size_t *smlen: pointer to output length of signed
*                               message
*              - const uint8_t *m: pointer to message to be signed
*              - size_t mlen: length of message
*              - const uint8_t *sk: pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
JNIEXPORT jint JNICALL Java_com_beechat_network_Dilithium_crypto_1sign
    (JNIEnv *env, jobject thisObject, jbyteArray _sm, jbyteArray _m, jlong _mlen, jbyteArray _sk)
{
  size_t i;
  int smlen = 0;

  UNUSED(thisObject);
  uint8_t *sm = (uint8_t*)malloc(_mlen + CRYPTO_BYTES);
  uint8_t *m = (uint8_t*)malloc(_mlen);
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  size_t mlen = _mlen;
  jbyte *_smj = (jbyte*)malloc((_mlen + CRYPTO_BYTES) * sizeof(jbyte));
  jbyte *_mj = (jbyte*)malloc(_mlen * sizeof(jbyte));
  jbyte _skj[CRYPTO_SECRETKEYBYTES];

  (*env)->GetByteArrayRegion(env, _m, 0, _mlen, _mj);
  (*env)->GetByteArrayRegion(env, _sk, 0, CRYPTO_SECRETKEYBYTES, _skj);
  for (size_t ii = 0; ii < mlen; ii++) m[ii] = _mj[ii];
  for (size_t ii = 0; ii < CRYPTO_SECRETKEYBYTES; ii++) sk[ii] = _skj[ii];

  for(i = 0; i < mlen; ++i)
    sm[CRYPTO_BYTES + mlen - 1 - i] = m[mlen - 1 - i];
  crypto_sign_signature(sm, (size_t*)&smlen, sm + CRYPTO_BYTES, mlen, sk);
  smlen += mlen;

  for (size_t ii = 0; ii < (size_t)smlen; ii++) _smj[ii] = sm[ii];
  (*env)->SetByteArrayRegion(env, _sm, 0, smlen, _smj);

  free(sm);
  free(m);
  free(_smj);
  //free(_mj);

  return smlen;
}

/*************************************************
* Name:        crypto_sign_verify
*
* Description: Verifies signature.
*
* Arguments:   - uint8_t *m: pointer to input signature
*              - size_t siglen: length of signature
*              - const uint8_t *m: pointer to message
*              - size_t mlen: length of message
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns 0 if signature could be verified correctly and -1 otherwise
**************************************************/
int crypto_sign_verify(const uint8_t *sig,
                       size_t siglen,
                       const uint8_t *m,
                       size_t mlen,
                       const uint8_t *pk)
{
  unsigned int i;
  uint8_t buf[K*POLYW1_PACKEDBYTES];
  uint8_t rho[SEEDBYTES];
  uint8_t mu[CRHBYTES];
  uint8_t c[SEEDBYTES];
  uint8_t c2[SEEDBYTES];
  poly cp;
  polyvecl mat[K], z;
  polyveck t1, w1, h;
  keccak_state state;

  if(siglen != CRYPTO_BYTES)
    return -1;

  unpack_pk(rho, &t1, pk);
  if(unpack_sig(c, &z, &h, sig))
    return -1;
  if(polyvecl_chknorm(&z, GAMMA1 - BETA))
    return -1;

  /* Compute CRH(H(rho, t1), msg) */
  shake256(mu, SEEDBYTES, pk, CRYPTO_PUBLICKEYBYTES);
  shake256_init(&state);
  shake256_absorb(&state, mu, SEEDBYTES);
  shake256_absorb(&state, m, mlen);
  shake256_finalize(&state);
  shake256_squeeze(mu, CRHBYTES, &state);

  /* Matrix-vector multiplication; compute Az - c2^dt1 */
  poly_challenge(&cp, c);
  polyvec_matrix_expand(mat, rho);

  polyvecl_ntt(&z);
  polyvec_matrix_pointwise_montgomery(&w1, mat, &z);

  poly_ntt(&cp);
  polyveck_shiftl(&t1);
  polyveck_ntt(&t1);
  polyveck_pointwise_poly_montgomery(&t1, &cp, &t1);

  polyveck_sub(&w1, &w1, &t1);
  polyveck_reduce(&w1);
  polyveck_invntt_tomont(&w1);

  /* Reconstruct w1 */
  polyveck_caddq(&w1);
  polyveck_use_hint(&w1, &w1, &h);
  polyveck_pack_w1(buf, &w1);

  /* Call random oracle and verify challenge */
  shake256_init(&state);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_absorb(&state, buf, K*POLYW1_PACKEDBYTES);
  shake256_finalize(&state);
  shake256_squeeze(c2, SEEDBYTES, &state);
  for(i = 0; i < SEEDBYTES; ++i)
    if(c[i] != c2[i])
      return -1;

  return 0;
}

/*************************************************
* Name:        crypto_sign_open
*
* Description: Verify signed message.
*
* Arguments:   - uint8_t *m: pointer to output message (allocated
*                            array with smlen bytes), can be equal to sm
*              - size_t *mlen: pointer to output length of message
*              - const uint8_t *sm: pointer to signed message
*              - size_t smlen: length of signed message
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns 0 if signed message could be verified correctly and -1 otherwise
**************************************************/
JNIEXPORT jint JNICALL Java_com_beechat_network_Dilithium_crypto_1sign_1open
    (JNIEnv *env, jobject thisObject, jbyteArray _m, jbyteArray _sm, jlong _smlen, jbyteArray _pk)
{
  size_t i;

  size_t smlen = _smlen;
  int mlen = 0;

  UNUSED(thisObject);
  uint8_t *sm = (uint8_t*)malloc(smlen);
  uint8_t *m = NULL;
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];

  jbyte *_smj = (jbyte*)malloc(smlen * sizeof(jbyte));
  jbyte *_mj = NULL;
  jbyte _pkj[CRYPTO_PUBLICKEYBYTES];

  (*env)->GetByteArrayRegion(env, _sm, 0, _smlen, _smj);
  (*env)->GetByteArrayRegion(env, _pk, 0, CRYPTO_PUBLICKEYBYTES, _pkj);
  for (size_t ii = 0; ii < smlen; ii++) sm[ii] = _smj[ii];
  for (size_t ii = 0; ii < CRYPTO_PUBLICKEYBYTES; ii++) pk[ii] = _pkj[ii];

  if(smlen < CRYPTO_BYTES)
    goto badsig;

  mlen = smlen - CRYPTO_BYTES;
  m = (uint8_t*)malloc(mlen);
  _mj = (jbyte*)malloc(mlen * sizeof(jbyte));
  if(crypto_sign_verify(sm, CRYPTO_BYTES, sm + CRYPTO_BYTES, mlen, pk))
    goto badsig;
  else {
    /* All good, copy msg, return 0 */
    for(i = 0; i < (size_t)mlen; ++i)
      m[i] = sm[CRYPTO_BYTES + i];

    for (size_t ii = 0; ii < (size_t)mlen; ii++) _mj[ii] = m[ii];
    (*env)->SetByteArrayRegion(env, _m, 0, mlen, _mj);
    free(sm);
    free(m);
    free(_smj);
    free(_mj);

    return mlen;
  }

badsig:
  /* Signature verification failed */
  for(i = 0; i < (size_t)mlen; ++i)
    m[i] = 0;

  for (size_t ii = 0; ii < (size_t)mlen; ii++) _mj[ii] = m[ii];
  (*env)->SetByteArrayRegion(env, _m, 0, mlen, _mj);
  free(sm);
  free(m);
  free(_smj);
  free(_mj);
  return -1;
}

