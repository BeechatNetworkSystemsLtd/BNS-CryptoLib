#include <pybind11/pybind11.h>

#include "base58.h"

#define SEEDBYTES 32
#define CRHBYTES 64
#define N 256
#define Q 8380417
#define D 13
#define ROOT_OF_UNITY 1753

#define K 4
#define L 4
#define ETA 2
#define TAU 39
#define BETA 78
#define GAMMA1 (1 << 17)
#define GAMMA2 ((Q-1)/88)
#define OMEGA 80

#define POLYT1_PACKEDBYTES  320
#define POLYT0_PACKEDBYTES  416
#define POLYVECH_PACKEDBYTES (OMEGA + K)

#if GAMMA1 == (1 << 17)
#define POLYZ_PACKEDBYTES   576
#elif GAMMA1 == (1 << 19)
#define POLYZ_PACKEDBYTES   640
#endif

#if GAMMA2 == (Q-1)/88
#define POLYW1_PACKEDBYTES  192
#elif GAMMA2 == (Q-1)/32
#define POLYW1_PACKEDBYTES  128
#endif

#if ETA == 2
#define POLYETA_PACKEDBYTES  96
#elif ETA == 4
#define POLYETA_PACKEDBYTES 128
#endif

#define CRYPTO_PUBLICKEYBYTES (SEEDBYTES + K*POLYT1_PACKEDBYTES)
#define CRYPTO_SECRETKEYBYTES (3*SEEDBYTES \
                               + L*POLYETA_PACKEDBYTES \
                               + K*POLYETA_PACKEDBYTES \
                               + K*POLYT0_PACKEDBYTES)
#define CRYPTO_BYTES (SEEDBYTES + L*POLYZ_PACKEDBYTES + POLYVECH_PACKEDBYTES)


typedef struct {
  int32_t coeffs[N];
} poly;


extern "C" int pqcrystals_dilithium2_ref_keypair(uint8_t *pk, uint8_t *sk);
extern "C" int pqcrystals_dilithium2_ref(
    uint8_t *sm
  , size_t *smlen
  , const uint8_t *m
  , size_t mlen
  , const uint8_t *sk
);
extern "C" int pqcrystals_dilithium2_ref_open(
    uint8_t *m
  , size_t *mlen
  , const uint8_t *sm
  , size_t smlen
  , const uint8_t *pk
);


int pk_ready = 0, sk_ready = 0, skey_ready = 0;
uint8_t pk[CRYPTO_PUBLICKEYBYTES] = { 0 };
uint8_t sk[CRYPTO_SECRETKEYBYTES] = { 0 };
uint8_t pk_base58[CRYPTO_PUBLICKEYBYTES * 2] = { 0 };
uint8_t sk_base58[CRYPTO_SECRETKEYBYTES * 2] = { 0 };

uint8_t *sm = nullptr;
uint8_t *sm_base58 = nullptr;
size_t sm_len = 0;


extern "C" int pqcrystals_dilithium2_ref_keypair_p()
{
    int ret = pqcrystals_dilithium2_ref_keypair(pk, sk);
    ret = encode_base58(pk, CRYPTO_PUBLICKEYBYTES, pk_base58, CRYPTO_PUBLICKEYBYTES * 2);
    pk_ready = ret ? 1 : 0;
    ret = encode_base58(sk, CRYPTO_SECRETKEYBYTES, sk_base58, CRYPTO_SECRETKEYBYTES * 2);
    sk_ready = ret ? 1 : 0;
    return ret ? 0 : 1;
}

extern "C" char const* pqcrystals_dilithium2_ref_p(
    char const *m
  , int mlen
){
    if (sm)
    {
        delete[] sm;
        delete[] sm_base58;
        sm = nullptr;
    }
    sm_len = static_cast<size_t>(mlen) + CRYPTO_BYTES;
    sm = new uint8_t[sm_len];
    sm_base58 = new uint8_t[sm_len * 2];

    pqcrystals_dilithium2_ref(
        sm
      , &sm_len
      , reinterpret_cast<uint8_t const*>(m)
      , static_cast<size_t>(mlen)
      , sk
    );

    encode_base58(sm, sm_len, sm_base58, sm_len * 2);
    return reinterpret_cast<char const*>(sm_base58);
}

extern "C" char const* pqcrystals_dilithium2_ref_open_p(
    char const *m
  , int mlen
)
{
    size_t mmlen = static_cast<size_t>(mlen);
    if (sm)
    {
        delete[] sm;
        delete[] sm_base58;
        sm = nullptr;
    }
    sm_len = static_cast<size_t>(mlen);
    sm = new uint8_t[sm_len];
    sm_base58 = new uint8_t[sm_len * 2];

    mlen = decode_base58(reinterpret_cast<unsigned char const*>(m), strlen(m), sm_base58);

    int status = pqcrystals_dilithium2_ref_open(sm, &mmlen, sm_base58, mlen, pk);
    sm[mmlen] = 0;

    if (status == 0)
    {
        return reinterpret_cast<char const*>(sm);
    }
    else
    {
        return nullptr;
    }
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



PYBIND11_MODULE(pydilithium, m) {
  m.doc() = R"doc(
        Python module
        -----------------------
        .. currentmodule:: pydilithium
        .. autosummary::
           :toctree: _generate

           add
           subtract
    )doc";

  m.def("pqcrystals_dilithium2_ref_keypair", &pqcrystals_dilithium2_ref_keypair_p, R"doc(
        Generates public and private key.

        Arguments:   - uint8_t *pk: pointer to output public key (allocated
                                    array of CRYPTO_PUBLICKEYBYTES bytes)
                     - uint8_t *sk: pointer to output private key (allocated
                                    array of CRYPTO_SECRETKEYBYTES bytes)
        return int
    )doc");
  m.def("pqcrystals_dilithium2_ref", &pqcrystals_dilithium2_ref_p, R"doc(
        Compute signed message.

        Arguments:   - uint8_t *sm: pointer to output signed message (allocated
                                    array with CRYPTO_BYTES + mlen bytes),
                                    can be equal to m
                     - size_t *smlen: pointer to output length of signed
                                      message
                     - const uint8_t *m: pointer to message to be signed
                     - size_t mlen: length of message
                     - const uint8_t *sk: pointer to bit-packed secret key
        return int
    )doc");

  m.def("pqcrystals_dilithium2_ref_open", &pqcrystals_dilithium2_ref_open_p, R"doc(
        Verify signed message.

        Arguments:   - uint8_t *m: pointer to output message (allocated
                                   array with smlen bytes), can be equal to sm
                     - size_t *mlen: pointer to output length of message
                     - const uint8_t *sm: pointer to signed message
                     - size_t smlen: length of signed message
                     - const uint8_t *pk: pointer to bit-packed public key
        return int
    )doc");
  m.def("pqcrystals_get_pk", &pqcrystals_get_pk, R"doc(
        Get public key.

        return public key
    )doc");
  m.def("pqcrystals_get_sk", &pqcrystals_get_sk, R"doc(
        Get secret key.

        return secret key
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
