#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_token_pool.h"
#endif

#include <stdlib.h>
#include <string.h>

static void jsonlite_token_pool_extend_capacity(jsonlite_token_pool pool, ptrdiff_t index);
static jsonlite_token_bucket terminate_bucket = {NULL, NULL, 0, 0, NULL};

size_t jsonlite_token_pool_batch_init(void *mem, size_t size, jsonlite_token_pool *pools) {
    size_t i, j;
    jsonlite_token_pool p = (jsonlite_token_pool)mem;
    size_t count = size / sizeof(jsonlite_token_pool_struct);
    for (i = 0; i < count; i++, p++, pools++) {
        *pools = p;
        p->content_pool = NULL;
        p->content_pool_size = 0;

        for (j = 0; j < JSONLITE_TOKEN_POOL_FRONT; j++) {
            p->blocks[j].buckets = &terminate_bucket;
            p->blocks[j].capacity = 0;
        }
    }

    return count;
}

void jsonlite_token_pool_copy_tokens(jsonlite_token_pool pool) {
    jsonlite_token_bucket *bucket;
    size_t size = 0;
    int i;
    for (i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        bucket = pool->blocks[i].buckets;
        while (bucket->start != NULL) {
            size += bucket->end - bucket->start;
            bucket++;
        }
    }

    if (size == pool->content_pool_size) {
        return;
    }

    uint8_t *buffer = (uint8_t *)malloc(size);
    uint8_t *p = buffer;
    for (i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        bucket = pool->blocks[i].buckets;
        while (bucket->start != NULL) {
            size_t length = bucket->end - bucket->start;
            memcpy(p, bucket->start, length);
            bucket->start = p;
            bucket->end = p + length;
            p += length;
            bucket++;
        }
    }

    free(pool->content_pool);
    pool->content_pool = buffer;
    pool->content_pool_size = size;
}

void jsonlite_token_pool_batch_cleanup(jsonlite_token_pool *pools, size_t count, jsonlite_token_pool_release_value_fn release) {
    size_t i, j;
    jsonlite_token_pool pool = *pools;
    for (i = 0; i < count; i++, pool++) {
        for (j = 0; j < JSONLITE_TOKEN_POOL_FRONT; j++) {
            jsonlite_token_bucket *bucket = pool->blocks[j].buckets;
            if (bucket->start == NULL) {
                continue;
            }

            if (release != NULL) {
                for (; bucket->start != NULL; bucket++) {
                    release((void *)bucket->value);
                }
            }

            free(pool->blocks[j].buckets);
            pool->blocks[j].buckets = &terminate_bucket;
            pool->blocks[j].capacity = 0;
        }

        free(pool->content_pool);
        pool->content_pool = NULL;
        pool->content_pool_size = 0;
    }
}

jsonlite_token_bucket *jsonlite_token_pool_get_bucket(jsonlite_token_pool pool, jsonlite_token *token) {
    ptrdiff_t length = token->end - token->start;
    uint32_t hash = jsonlite_hash(token->start, (size_t)length);
    ptrdiff_t index = hash & JSONLITE_TOKEN_POOL_FRONT_MASK;
    size_t count = 0;
    jsonlite_token_bucket *bucket = pool->blocks[index].buckets;
    for (; bucket->start != NULL; count++, bucket++) {
        if (bucket->token_hash != hash) {
            continue;
        }

        if (length != bucket->end - bucket->start) {
            continue;
        }

        if (memcmp(token->start, bucket->start, (size_t)length) == 0) {
            return bucket;
        }
    }

    size_t capacity = pool->blocks[index].capacity;
    if (count + 1 >= capacity) {
        jsonlite_token_pool_extend_capacity(pool, index);
    }

    bucket = pool->blocks[index].buckets + count;
    bucket->token_hash = hash;
    bucket->start = token->start;
    bucket->end = token->end;
    bucket->value = NULL;
    bucket[1].start = NULL;
    return bucket;
}

static void jsonlite_token_pool_extend_capacity(jsonlite_token_pool pool, ptrdiff_t index) {
    size_t capacity = pool->blocks[index].capacity;
    if (capacity == 0) {
        capacity = 0x10;
    }

    size_t size = capacity * sizeof(jsonlite_token_bucket);
    jsonlite_token_bucket *buckets = pool->blocks[index].buckets;
    jsonlite_token_bucket *extended = (jsonlite_token_bucket *)malloc(2 * size);

    if (buckets->start != NULL) {
        memcpy(extended, buckets, size);
        free(buckets);
    }

    pool->blocks[index].buckets = extended;
    pool->blocks[index].capacity = 2 * capacity;
}

// Used MurmurHash2 function by Austin Appleby
// http://code.google.com/p/smhasher/ revision 147

//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

static uint32_t MurmurHash2(const void *key, size_t len) {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    // Initialize the token_hash to a 'random' value

    uint32_t h = (uint32_t)len;

    // Mix 4 bytes at a time into the token_hash

    const unsigned char *data = (const unsigned char *)key;

    while (len >= 4) {
        uint32_t k = *(uint32_t *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch (len) {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    // Do a few final mixes of the token_hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

//-----------------------------------------------------------------------------
uint32_t jsonlite_hash(const void *data, size_t len) {
    return MurmurHash2(data, len);
}
