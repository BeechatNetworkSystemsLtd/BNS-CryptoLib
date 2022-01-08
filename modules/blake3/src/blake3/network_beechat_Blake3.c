#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "network_beechat_Blake3.h"
#include "blake3_impl.h"

INLINE void chunk_state_init(blake3_chunk_state *self, const uint32_t key[8],
                             uint8_t flags) {
  memcpy(self->cv, key, BLAKE3_KEY_LEN);
  self->chunk_counter = 0;
  memset(self->buf, 0, BLAKE3_BLOCK_LEN);
  self->buf_len = 0;
  self->blocks_compressed = 0;
  self->flags = flags;
}

INLINE void hasher_init_base(blake3_hasher *self, const uint32_t key[8],
                             uint8_t flags) {
  memcpy(self->key, key, BLAKE3_KEY_LEN);
  chunk_state_init(&self->chunk, key, flags);
  self->cv_stack_len = 0;
}

JNIEXPORT jint JNICALL Java_com_beechat_network_Blake3_init(
    JNIEnv *env
  , jobject obj
){
    blake3_hasher self = { 0 };
    //java_to_c_object(&self, env, obj);

    memcpy(self.key, IV, BLAKE3_KEY_LEN);
    chunk_state_init(&(self.chunk), IV, 0);
    self.cv_stack_len = 0;

    c_to_java_object(&self, env, obj);
    return 0;
}

JNIEXPORT jint JNICALL Java_com_beechat_network_Blake3_init_1keyed(
    JNIEnv *env
  , jobject obj
  , jbyteArray _key
){
    uint32_t key_words[8];
    blake3_hasher self = { 0 };

    uint8_t key[8] = { 0 };
    jbyte key_j[8] = { 0 };
    (*env)->GetByteArrayRegion(env, _key, 0, 8, key_j);
    for (size_t i = 0; i < 8; i++) key[i] = key_j[i];

    load_key_words(key, key_words);
    hasher_init_base(&self, key_words, KEYED_HASH);

    c_to_java_object(&self, env, obj);
    return 0;
}

JNIEXPORT jint JNICALL Java_com_beechat_network_Blake3_init_1derive_1key(
    JNIEnv *env
  , jobject obj
  , jcharArray _context
  , jint _len
){
    jboolean is_copy;
    blake3_hasher context_hasher;
    blake3_hasher self = { 0 };
    int len  = _len;

    char *context = (*env)->GetPrimitiveArrayCritical(env, _context, &is_copy);

    hasher_init_base(&context_hasher, IV, DERIVE_KEY_CONTEXT);
    blake3_hasher_update(&context_hasher, context, len);
    uint8_t context_key[BLAKE3_KEY_LEN];
    blake3_hasher_finalize(&context_hasher, context_key, BLAKE3_KEY_LEN);
    uint32_t context_key_words[8];
    load_key_words(context_key, context_key_words);
    hasher_init_base(&self, context_key_words, DERIVE_KEY_MATERIAL);

    (*env)->ReleasePrimitiveArrayCritical(env, _context, context, JNI_ABORT);
    c_to_java_object(&self, env, obj);
    return 0;
}


JNIEXPORT void JNICALL Java_com_beechat_network_Blake3_update_1native
  (JNIEnv *env, jobject obj, jbyteArray _context, jint _len)
{
    jboolean is_copy;
    blake3_hasher self = { 0 };
    java_to_c_object(&self, env, obj);
    int len  = _len;
    char *context = (*env)->GetPrimitiveArrayCritical(env, _context, &is_copy);

    blake3_hasher_update(&self, context, len);

    (*env)->ReleasePrimitiveArrayCritical(env, _context, context, JNI_ABORT);
    c_to_java_object(&self, env, obj);
}


JNIEXPORT void JNICALL Java_com_beechat_network_Blake3_finalize_1seek_1native
  (JNIEnv *env, jobject obj, jlong _seek, jbyteArray _output, jint _outlen)
{
    blake3_hasher self = { 0 };
    jboolean is_copy;
    uint64_t seek = _seek;
    size_t outlen = _outlen;

    java_to_c_object(&self, env, obj);

    uint8_t *output = (*env)->GetPrimitiveArrayCritical(env, _output, &is_copy);

    blake3_hasher_finalize_seek(&self, seek, output, outlen);

    (*env)->ReleasePrimitiveArrayCritical(env, _output, output, JNI_ABORT);
    c_to_java_object(&self, env, obj);
}


void java_to_c_object(blake3_hasher *str, JNIEnv *env, jobject obj)
{
    jfieldID fid = { 0 };
    jclass cls;
    jboolean is_copy;
    jint cv_stack_len_j;
    jbyte buf_len_j, blocks_compressed_j, flags_j;
    jlong chunk_counter_j;

    if (!str)
    {
        return;
    }

    cls = (*env)->GetObjectClass(env, obj);

    fid = (*env)->GetFieldID(env, cls, "key", "[I");
    jintArray key_a = (*env)->GetObjectField(env, obj, fid);
    jint *key_c = (*env)->GetPrimitiveArrayCritical(env, key_a, &is_copy);
    memcpy(str->key, key_c, 8 * sizeof(int));
    (*env)->ReleasePrimitiveArrayCritical(env, key_a, key_c, JNI_ABORT);

    fid = (*env)->GetFieldID(env, cls, "cv", "[I");
    jintArray cv_a = (*env)->GetObjectField(env, obj, fid);
    jint *cv_c = (*env)->GetPrimitiveArrayCritical(env, cv_a, &is_copy);
    memcpy(str->chunk.cv, cv_c, 8 * sizeof(int));
    (*env)->ReleasePrimitiveArrayCritical(env, cv_a, cv_c, JNI_ABORT);

    fid = (*env)->GetFieldID(env, cls, "cv_stack_len", "I");
    cv_stack_len_j = (*env)->GetIntField(env, obj, fid);
    str->cv_stack_len = cv_stack_len_j;

    fid = (*env)->GetFieldID(env, cls, "chunk_counter", "J");
    chunk_counter_j = (*env)->GetLongField(env, obj, fid);
    str->chunk.chunk_counter = chunk_counter_j;

    fid = (*env)->GetFieldID(env, cls, "buf", "[B");
    jbyteArray buf_a = (*env)->GetObjectField(env, obj, fid);
    jbyte *buf_c = (*env)->GetPrimitiveArrayCritical(env, buf_a, &is_copy);
    memcpy(str->chunk.buf, buf_c, BLAKE3_BLOCK_LEN);
    (*env)->ReleasePrimitiveArrayCritical(env, buf_a, buf_c, JNI_ABORT);

    fid = (*env)->GetFieldID(env, cls, "buf_len", "B");
    buf_len_j = (*env)->GetByteField(env, obj, fid);
    str->chunk.buf_len = buf_len_j;

    fid = (*env)->GetFieldID(env, cls, "blocks_compressed", "B");
    blocks_compressed_j = (*env)->GetByteField(env, obj, fid);
    str->chunk.blocks_compressed = blocks_compressed_j;

    fid = (*env)->GetFieldID(env, cls, "flags", "B");
    flags_j = (*env)->GetByteField(env, obj, fid);
    str->chunk.flags = flags_j;

    fid = (*env)->GetFieldID(env, cls, "cv_stack", "[B");
    jbyteArray cv_stack_a = (*env)->GetObjectField(env, obj, fid);
    jbyte *cv_stack_c = (*env)->GetPrimitiveArrayCritical(env, cv_stack_a, &is_copy);
    memcpy(str->cv_stack, cv_stack_c, (BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN);
    (*env)->ReleasePrimitiveArrayCritical(env, cv_stack_a, cv_stack_c, JNI_ABORT);
}

void c_to_java_object(blake3_hasher *str, JNIEnv *env, jobject obj)
{
    jfieldID fid = { 0 };
    jboolean is_copy;

    jint cv_stack_len_j;
    jbyte buf_len_j, blocks_compressed_j, flags_j;
    jlong chunk_counter_j;

    if (!str)
    {
        return;
    }

    jclass cls = (*env)->GetObjectClass(env, obj);

    fid = (*env)->GetFieldID(env, cls, "key", "[I");
    jintArray key_a = (*env)->GetObjectField(env, obj, fid);
    jint *key_c = (*env)->GetPrimitiveArrayCritical(env, key_a, &is_copy);
    memcpy(key_c, str->key, 8 * sizeof(int));
    (*env)->ReleasePrimitiveArrayCritical(env, key_a, key_c, JNI_ABORT);

    fid = (*env)->GetFieldID(env, cls, "cv", "[I");
    jintArray cv_a = (*env)->GetObjectField(env, obj, fid);
    jint *cv_c = (*env)->GetPrimitiveArrayCritical(env, cv_a, &is_copy);
    memcpy(cv_c, str->chunk.cv, 8 * sizeof(int));
    (*env)->ReleasePrimitiveArrayCritical(env, cv_a, cv_c, JNI_ABORT);

    fid = (*env)->GetFieldID(env, cls, "cv_stack_len", "I");
    cv_stack_len_j = str->cv_stack_len;
    (*env)->SetIntField(env, obj, fid, cv_stack_len_j);

    fid = (*env)->GetFieldID(env, cls, "chunk_counter", "J");
    chunk_counter_j = str->chunk.chunk_counter;
    (*env)->SetLongField(env, obj, fid, chunk_counter_j);

    fid = (*env)->GetFieldID(env, cls, "buf", "[B");
    jbyteArray buf_a = (*env)->GetObjectField(env, obj, fid);
    jbyte *buf_c = (*env)->GetPrimitiveArrayCritical(env, buf_a, &is_copy);
    memcpy(buf_c, str->chunk.buf, BLAKE3_BLOCK_LEN);
    (*env)->ReleasePrimitiveArrayCritical(env, buf_a, buf_c, JNI_ABORT);

    fid = (*env)->GetFieldID(env, cls, "buf_len", "B");
    buf_len_j = str->chunk.buf_len;
    (*env)->SetByteField(env, obj, fid, buf_len_j);

    fid = (*env)->GetFieldID(env, cls, "blocks_compressed", "B");
    blocks_compressed_j = str->chunk.blocks_compressed;
    (*env)->SetByteField(env, obj, fid, blocks_compressed_j);

    fid = (*env)->GetFieldID(env, cls, "flags", "B");
    flags_j = str->chunk.flags;
    (*env)->SetByteField(env, obj, fid, flags_j);

    fid = (*env)->GetFieldID(env, cls, "cv_stack", "[B");
    jbyteArray cv_stack_a = (*env)->GetObjectField(env, obj, fid);
    jbyte *cv_stack_c = (*env)->GetPrimitiveArrayCritical(env, cv_stack_a, &is_copy);
    memcpy(cv_stack_c, str->cv_stack, (BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN);

    (*env)->ReleasePrimitiveArrayCritical(env, cv_stack_a, cv_stack_c, JNI_ABORT);
}

