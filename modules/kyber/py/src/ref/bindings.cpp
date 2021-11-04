#include <pybind11/pybind11.h>

#include "base58.h"

#define KYBER_N 256
#define KYBER_Q 3329

#define KYBER_SYMBYTES 32   /* size in bytes of hashes, and seeds */
#define KYBER_SSBYTES  32   /* size in bytes of shared key */

#define KYBER_POLYBYTES		384
#define KYBER_POLYVECBYTES	(KYBER_K * KYBER_POLYBYTES)

#define KYBER_ETA1 3
#define KYBER_POLYCOMPRESSEDBYTES    128
#define KYBER_POLYVECCOMPRESSEDBYTES (KYBER_K * 320)

#define KYBER_ETA2 2

#define KYBER_INDCPA_MSGBYTES       (KYBER_SYMBYTES)
#define KYBER_INDCPA_PUBLICKEYBYTES (KYBER_POLYVECBYTES + KYBER_SYMBYTES)
#define KYBER_INDCPA_SECRETKEYBYTES (KYBER_POLYVECBYTES)
#define KYBER_INDCPA_BYTES          (KYBER_POLYVECCOMPRESSEDBYTES + KYBER_POLYCOMPRESSEDBYTES)

#define KYBER_PUBLICKEYBYTES  (KYBER_INDCPA_PUBLICKEYBYTES)
/* 32 bytes of additional space to save H(pk) */
#define KYBER_SECRETKEYBYTES  (KYBER_INDCPA_SECRETKEYBYTES + KYBER_INDCPA_PUBLICKEYBYTES + 2*KYBER_SYMBYTES)
#define KYBER_CIPHERTEXTBYTES (KYBER_INDCPA_BYTES)


extern "C" int pqcrystals_kyber512_ref_keypair(uint8_t *pk, uint8_t *sk);

extern "C" int pqcrystals_kyber512_ref_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk);

extern "C" int pqcrystals_kyber512_ref_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk);

int pk_ready = 0, sk_ready = 0, skey_ready = 0;
uint8_t pk[KYBER_PUBLICKEYBYTES] = { 0 };
uint8_t sk[KYBER_SECRETKEYBYTES] = { 0 };
uint8_t pk_base58[KYBER_PUBLICKEYBYTES * 2] = { 0 };
uint8_t sk_base58[KYBER_SECRETKEYBYTES * 2] = { 0 };
uint8_t skey[KYBER_SSBYTES] = { 0 };
uint8_t skey_base58[KYBER_SSBYTES * 2] = { 0 };
uint8_t ct[KYBER_CIPHERTEXTBYTES] = { 0 };
uint8_t ct_base58[KYBER_CIPHERTEXTBYTES * 2] = { 0 };

extern "C" int pqcrystals_kyber512_ref_keypair_p()
{
    int ret = pqcrystals_kyber512_ref_keypair(pk, sk);
    ret = encode_base58(pk, KYBER_PUBLICKEYBYTES, pk_base58, KYBER_PUBLICKEYBYTES * 2);
    pk_ready = ret ? 1 : 0;
    ret = encode_base58(sk, KYBER_SECRETKEYBYTES, sk_base58, KYBER_SECRETKEYBYTES * 2);
    sk_ready = ret ? 1 : 0;
    return ret ? 0 : 1;
}


extern "C" int pqcrystals_kyber512_ref_enc_p(char const *text, char const *key)
{
    int ret = 0;
    ret = decode_base58(reinterpret_cast<unsigned char const*>(text), strlen(text), ct);
    ret = decode_base58(reinterpret_cast<unsigned char const*>(key), strlen(key), pk);
    ret = pqcrystals_kyber512_ref_enc(ct, skey, pk);
    ret = encode_base58(skey, KYBER_SSBYTES, skey_base58, KYBER_SSBYTES * 2);
    ret = encode_base58(ct, KYBER_CIPHERTEXTBYTES, ct_base58, KYBER_CIPHERTEXTBYTES * 2);
    skey_ready = 1;
    return ret;
}

extern "C" int pqcrystals_kyber512_ref_dec_p(char const *text, char const *key)
{
    int ret = 0;

    ret = decode_base58(reinterpret_cast<unsigned char const*>(text), strlen(text), ct);
    ret = decode_base58(reinterpret_cast<unsigned char const*>(key), strlen(key), sk);
    ret = pqcrystals_kyber512_ref_dec(skey, ct, sk);
    ret = encode_base58(skey, KYBER_SSBYTES, skey_base58, KYBER_SSBYTES * 2);
    skey_ready = 1;

    return ret;
}


extern "C" void pqcrystals_set_pk(char const* m)
{
    decode_base58(reinterpret_cast<unsigned char const*>(m), strlen(m), pk);
}

extern "C" char const* pqcrystals_get_pk()
{
    if (pk_ready)
    {
        return reinterpret_cast<char*>(pk_base58);
    }
    else
    {
        return "";
    }
}

extern "C" void pqcrystals_set_sk(char const* m)
{
    decode_base58(reinterpret_cast<unsigned char const*>(m), strlen(m), sk);
}

extern "C" char const* pqcrystals_get_sk()
{
    if (sk_ready)
    {
        return reinterpret_cast<char*>(sk_base58);
    }
    else
    {
        return "";
    }
}

extern "C" char const* pqcrystals_get_skey()
{
    if (skey_ready)
    {
        return reinterpret_cast<char*>(skey_base58);
    }
    else
    {
        return "";
    }
}

extern "C" char const* pqcrystals_get_ct()
{
    return reinterpret_cast<char*>(ct_base58);
}


PYBIND11_MODULE(pykyber, m) {
  m.doc() = R"doc(
        Python module
        -----------------------
        .. currentmodule:: pykyber
        .. autosummary::
           :toctree: _generate

           add
           subtract
    )doc";

  m.def("pqcrystals_get_pk", &pqcrystals_get_pk, R"doc(
        Get public key base58 encoded string

        args: void
        return base58string
    )doc");
  m.def("pqcrystals_get_sk", &pqcrystals_get_sk, R"doc(
        Get secret key base58 encoded string

        args: void
        return base58string
    )doc");

  m.def("pqcrystals_get_skey", &pqcrystals_get_skey, R"doc(
        Get shared key base58 encoded string

        args: void
        return base58string
    )doc");

  m.def("pqcrystals_get_ct", &pqcrystals_get_ct, R"doc(
        Get chipher text base58 encoded string

        args: void
        return base58string
    )doc");

  m.def("pqcrystals_kyber512_ref_keypair", &pqcrystals_kyber512_ref_keypair_p, R"doc(
        512 bit version of Kyber

        Key pair generator.
    )doc");

  m.def("pqcrystals_kyber512_ref_enc", &pqcrystals_kyber512_ref_enc_p, R"doc(
        512 bit version of Kyber

        Encryptor
    )doc");

  m.def("pqcrystals_kyber512_ref_dec", &pqcrystals_kyber512_ref_dec_p, R"doc(
        512 bit version of Kyber

        Decyptor
    )doc");

  m.def("pqcrystals_set_pk", &pqcrystals_set_pk, R"doc(
        Set public key.

        return void
    )doc");
  m.def("pqcrystals_set_sk", &pqcrystals_set_sk, R"doc(
        Set secret key.

        return void
    )doc");

}
