#include <assert.h>
#include <stdbool.h>
#include <string.h>

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

JNIEXPORT jint JNICALL Java_network_beechat_Blake3_init(
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

JNIEXPORT jint JNICALL Java_network_beechat_Blake3_init_1keyed(
    JNIEnv *env
  , jobject obj
  , jbyteArray _key
){
    uint32_t key_words[8];
    blake3_hasher self = { 0 };
    //java_to_c_object(&self, env, obj);

    uint8_t key[8] = { 0 };
    jbyte key_j[8] = { 0 };
    (*env)->GetByteArrayRegion(env, _key, 0, 8, key_j);
    for (size_t i = 0; i < 8; i++) key[i] = key_j[i];

    load_key_words(key, key_words);
    hasher_init_base(&self, key_words, KEYED_HASH);

    c_to_java_object(&self, env, obj);
    return 0;
}

JNIEXPORT jint JNICALL Java_network_beechat_Blake3_init_1derive_1key(
    JNIEnv *env
  , jobject obj
  , jcharArray _context
  , jint _len
){
    blake3_hasher context_hasher;
    blake3_hasher self = { 0 };
    int len  = _len;

    char *context = (char *)malloc(len);
    jbyte *context_j = (jbyte *)malloc(len);
    (*env)->GetByteArrayRegion(env, _context, 0, len, context_j);
    for (size_t i = 0; i < 8; i++) context[i] = context_j[i];

    hasher_init_base(&context_hasher, IV, DERIVE_KEY_CONTEXT);
    blake3_hasher_update(&context_hasher, context, len);
    uint8_t context_key[BLAKE3_KEY_LEN];
    blake3_hasher_finalize(&context_hasher, context_key, BLAKE3_KEY_LEN);
    uint32_t context_key_words[8];
    load_key_words(context_key, context_key_words);
    hasher_init_base(&self, context_key_words, DERIVE_KEY_MATERIAL);

    free(context);
    free(context_j);
    c_to_java_object(&self, env, obj);
    return 0;
}

JNIEXPORT void JNICALL Java_network_beechat_Blake3_update_1native
  (JNIEnv *env, jobject obj, jbyteArray _context, jint _len)
{
    blake3_hasher self = { 0 };
    java_to_c_object(&self, env, obj);
    int len  = _len;

    char *context = (char *)malloc(len);
    jbyte *context_j = (jbyte *)malloc(len);
    (*env)->GetByteArrayRegion(env, _context, 0, len, context_j);
    for (size_t i = 0; i < 8; i++) context[i] = context_j[i];

    blake3_hasher_update(&self, context, len);

    free(context);
    free(context_j);
    c_to_java_object(&self, env, obj);
}


JNIEXPORT void JNICALL Java_network_beechat_Blake3_finalize_1seek_1native
  (JNIEnv *env, jobject obj, jlong _seek, jbyteArray _output, jint _outlen)
{
    blake3_hasher self = { 0 };
    uint64_t seek = _seek;
    size_t outlen = _outlen;
    uint8_t *output = (uint8_t *)malloc(outlen);
    jbyte *output_j = (jbyte *)malloc(outlen);

    java_to_c_object(&self, env, obj);

    blake3_hasher_finalize_seek(&self, seek, output, outlen);
    for (size_t i = 0; i < outlen; i++) output_j[i] = output[i];
    (*env)->GetByteArrayRegion(env, _output, 0, outlen, output_j);

    c_to_java_object(&self, env, obj);
    free(output);
    free(output_j);
}


void java_to_c_object(blake3_hasher *str, JNIEnv *env, jobject obj)
{
    jfieldID fid = { 0 };
    jclass cls;
    jintArray *key_data, *cv_data;
    jbyteArray *buf_data, *cv_stack_data;
    jint key_j[8], cv_j[8];
    jbyte buf_j[BLAKE3_BLOCK_LEN], cv_stack_j[(BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN];
    jbyte cv_stack_len_j, buf_len_j, blocks_compressed_j, flags_j;
    jlong chunk_counter_j;

    if (!str)
    {
        return;
    }

    cls = (*env)->GetObjectClass(env, obj);

    fid = (*env)->GetFieldID(env, cls, "key", "I");
    key_data = (jintArray*)((*env)->GetObjectField(env, obj, fid));
    (*env)->GetIntArrayRegion(env, *key_data, 0, 8, key_j);
    for (size_t i = 0; i < 8; ++i) str->key[i] = key_j[i];

    fid = (*env)->GetFieldID(env, cls, "cv", "I");
    cv_data = (jintArray*)((*env)->GetObjectField(env, obj, fid));
    (*env)->GetIntArrayRegion(env, *cv_data, 0, 8, cv_j);
    for (size_t i = 0; i < 8; ++i) str->chunk.cv[i] = cv_j[i];

    fid = (*env)->GetFieldID(env, cls, "cv_stack_len", "I");
    cv_stack_len_j = (*env)->GetByteField(env, obj, fid);
    str->cv_stack_len = cv_stack_len_j;

    fid = (*env)->GetFieldID(env, cls, "chunk_counter", "I");
    chunk_counter_j = (*env)->GetLongField(env, obj, fid);
    str->chunk.chunk_counter = chunk_counter_j;

    fid = (*env)->GetFieldID(env, cls, "buf", "I");
    buf_data = (jbyteArray*)((*env)->GetObjectField(env, obj, fid));
    (*env)->GetByteArrayRegion(env, *buf_data, 0, BLAKE3_BLOCK_LEN, buf_j);
    for (size_t i = 0; i < BLAKE3_BLOCK_LEN; ++i) str->chunk.buf[i] = buf_j[i];

    fid = (*env)->GetFieldID(env, cls, "buf_len", "I");
    buf_len_j = (*env)->GetByteField(env, obj, fid);
    str->chunk.buf_len = buf_len_j;

    fid = (*env)->GetFieldID(env, cls, "blocks_compressed", "I");
    blocks_compressed_j = (*env)->GetByteField(env, obj, fid);
    str->chunk.blocks_compressed = blocks_compressed_j;

    fid = (*env)->GetFieldID(env, cls, "flags", "I");
    flags_j = (*env)->GetByteField(env, obj, fid);
    str->chunk.flags = flags_j;

    fid = (*env)->GetFieldID(env, cls, "cv_stack", "I");
    cv_stack_data = (jbyteArray*)((*env)->GetObjectField(env, obj, fid));
    (*env)->GetByteArrayRegion(env, *cv_stack_data, 0, (BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN, cv_stack_j);
    for (size_t i = 0; i < (BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN; ++i) str->cv_stack[i] = cv_stack_j[i];

    /*(*env)->ReleaseIntArrayElements(key_data, key, 0);
    (*env)->ReleaseIntArrayElements(cv_data, cv, 0);
    (*env)->ReleaseByteArrayElements(buf_data, buf, 0);
    (*env)->ReleaseByteArrayElements(cv_stack_data, cv_stack, 0);*/
}

void c_to_java_object(blake3_hasher *str, JNIEnv *env, jobject obj)
{
    jfieldID fid = { 0 };

    jintArray *key_data, *cv_data;
    jbyteArray *buf_data, *cv_stack_data;
    jint key_j[8], cv_j[8];
    jbyte buf_j[BLAKE3_BLOCK_LEN], cv_stack_j[(BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN];
    jbyte cv_stack_len_j, buf_len_j, blocks_compressed_j, flags_j;
    jlong chunk_counter_j;

    if (!str)
    {
        return;
    }

    jclass cls = (*env)->GetObjectClass(env, obj);

    fid = (*env)->GetFieldID(env, cls, "key", "I");
    key_data = (jintArray*)((*env)->GetObjectField(env, obj, fid));
    for (size_t i = 0; i < 8; ++i) key_j[i] = str->key[i];
    (*env)->SetIntArrayRegion(env, *key_data, 0, 8, key_j);

    fid = (*env)->GetFieldID(env, cls, "cv", "I");
    cv_data = (jintArray*)((*env)->GetObjectField(env, obj, fid));
    for (size_t i = 0; i < 8; ++i) cv_j[i] = str->chunk.cv[i];
    (*env)->SetIntArrayRegion(env, *cv_data, 0, 8, cv_j);

    fid = (*env)->GetFieldID(env, cls, "cv_stack_len", "I");
    cv_stack_len_j = str->cv_stack_len;
    (*env)->SetByteField(env, obj, fid, cv_stack_len_j);

    fid = (*env)->GetFieldID(env, cls, "chunk_counter", "I");
    chunk_counter_j = str->chunk.chunk_counter;
    (*env)->SetLongField(env, obj, fid, chunk_counter_j);

    fid = (*env)->GetFieldID(env, cls, "buf", "I");
    buf_data = (jbyteArray*)((*env)->GetObjectField(env, obj, fid));
    for (size_t i = 0; i < BLAKE3_BLOCK_LEN; ++i) buf_j[i] = str->chunk.buf[i];
    (*env)->SetByteArrayRegion(env, *buf_data, 0, BLAKE3_BLOCK_LEN, buf_j);

    fid = (*env)->GetFieldID(env, cls, "buf_len", "I");
    buf_len_j = str->chunk.buf_len;
    (*env)->SetByteField(env, obj, fid, buf_len_j);

    fid = (*env)->GetFieldID(env, cls, "blocks_compressed", "I");
    blocks_compressed_j = str->chunk.blocks_compressed;
    (*env)->SetByteField(env, obj, fid, blocks_compressed_j);

    fid = (*env)->GetFieldID(env, cls, "flags", "I");
    flags_j = str->chunk.flags;
    (*env)->SetByteField(env, obj, fid, flags_j);

    fid = (*env)->GetFieldID(env, cls, "cv_stack", "I");
    cv_stack_data = (jbyteArray*)((*env)->GetObjectField(env, obj, fid));
    for (size_t i = 0; i < (BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN; ++i) cv_stack_j[i] = str->cv_stack[i];
    (*env)->SetByteArrayRegion(env, *cv_stack_data, 0, (BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN, cv_stack_j);

    /*env->ReleaseIntArrayElements(key_data, key, 0);
    env->ReleaseIntArrayElements(cv_data, cv, 0);
    env->ReleaseByteArrayElements(buf_data, buf, 0);
    env->ReleaseByteArrayElements(cv_stack_data, cv_stack, 0);*/
}

