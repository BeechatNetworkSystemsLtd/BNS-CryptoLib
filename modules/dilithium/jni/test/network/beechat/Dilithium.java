package network.beechat;

public class Dilithium {

    static final int CRYPTO_PUBLICKEYBYTES = 1312;
    static final int CRYPTO_SECRETKEYBYTES = 2528;
    static final int CRYPTO_BYTES = 2420;

    static {
        System.loadLibrary("dilithium2_ref_jni");
    }

    // Declare a native methods
    public static native int crypto_sign_keypair(byte []pk, byte []sk);
    public static native int crypto_sign(byte []sm, byte []m, long mlen, byte []sk);
    public static native int crypto_sign_open(byte []m, byte []sm, long smlen, byte []pk);

}

