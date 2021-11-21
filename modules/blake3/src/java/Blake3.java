package network.beechat;

public class Blake3 {

    static final String VERSION_STRING = "1.1.0";
    public static final int KEY_LEN = 32;
    public static final int OUT_LEN = 32;
    public static final int BLOCK_LEN = 64;
    public static final int CHUNK_LEN = 1024;
    public static final int MAX_DEPTH = 54;

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
        System.loadLibrary("blake3_jni");
    }

    private native int init();
    private native int init_keyed(byte []key);
    private native int init_derive_key(char []context, int len);
    private native void update_native(byte []input, int input_len);
    private native void finalize_seek_native(long seek, byte []output, int output_len);

    private void memAlloc() {
        key = new int[8];
        cv = new int[8];
        cv_stack_len = (MAX_DEPTH + 1) * OUT_LEN;
        cv_stack = new byte[cv_stack_len];
        buf = new byte[BLOCK_LEN];
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

        if (init_derive_key(context.toCharArray(), context.length()) != 0) {
            throw new Exception("Blake3 native init failed");
        }
    }

    public String getVersion() { return VERSION_STRING; }

    public void update(String input) {
        update_native(input.getBytes(), input.length());
    }

    public void update(byte[] input, int len) {
        update_native(input, len);
    }

    public String finalize(int size) {
        return finalize(size, 0L);
    }

    public String finalize(int size, long seek) {
        byte []output = new byte[size];
        finalize_seek_native(seek, output, size);

        String result = new String();
        for (byte c : output) {
            result += String.format("%02X", c);
        }

        return result;
    }

}

