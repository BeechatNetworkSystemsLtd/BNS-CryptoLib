package network.beechat;

import java.nio.charset.Charset;

public class Blake3 {

    static final String BLAKE3_VERSION_STRING = "1.1.0";
    static final int BLAKE3_KEY_LEN = 32;
    static final int BLAKE3_OUT_LEN = 32;
    static final int BLAKE3_BLOCK_LEN = 64;
    static final int BLAKE3_CHUNK_LEN = 1024;
    static final int BLAKE3_MAX_DEPTH = 54;

    int key[];
    int cv_stack_len;
    byte cv_stack[];

    int cv[];
    long chunk_counter;
    byte buf[];
    byte buf_len;
    byte blocks_compressed;
    byte flags;

    static {
        System.loadLibrary("blake3_x86_jni");
    }

    private native int init();
    private native int init_keyed(byte []key);
    private native int init_derive_key(char []context, int len);
    private native void update_native(byte []input, int input_len);
    private native void finalize_seek_native(long seek, byte []output, int output_len);

    private void memAlloc() {
        key = new int[8];
        cv = new int[8];
        cv_stack_len = (BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN;
        cv_stack = new byte[cv_stack_len];
        buf = new byte[BLAKE3_BLOCK_LEN];
    }

    public Blake3() throws Exception {
        memAlloc();

        if (init() != 0) {
            throw new Exception("Blake3 native init failed");
        }
    }

    public Blake3(byte []key) throws Exception {
        memAlloc();

        if (init_keyed(key) != 0) {
            throw new Exception("Blake3 native init failed");
        }
    }

    public Blake3(String context) throws Exception {
        memAlloc();

        if (init_derive_key(context.getCharArray(), init_derive_key.length()) != 0) {
            throw new Exception("Blake3 native init failed");
        }
    }

    public String getVersion() { return BLAKE3_VERSION_STRING; }

    public void update(String input) {
        update_native(input.getBytes(), input.length());
    }

    public String finalize(int size) {
        return finalize(size, 0L);
    }

    public String finalize(int size, long seek) {
        byte []output = new byte[size + 1];
        finalize_seek_native(seek, output, size);
        return new String(output, new Charset("ISO-8859-1"));
    }

}

