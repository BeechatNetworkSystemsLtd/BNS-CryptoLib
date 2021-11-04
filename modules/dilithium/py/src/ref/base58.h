

// reslen is the allocated length for result, feel free to overallocate
extern "C" int encode_base58(const unsigned char *source, int len, unsigned char result[], int reslen);

// result must be declared (for the worst case): char result[len * 2];
extern "C" int decode_base58(const unsigned char *str, int len, unsigned char *result);

