//
//  Copyright 2012-2013, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_AMALGAMATED
#include "../include/jsonlite_token_pool.h"
#endif

#include <stdlib.h>
#include <string.h>

#define JSONLITE_TOKEN_POOL_FRONT 0x80
#define JSONLITE_TOKEN_POOL_FRONT_MASK (JSONLITE_TOKEN_POOL_FRONT - 1)

typedef struct jsonlite_token_block {
    jsonlite_token_bucket *buckets;
    size_t capacity;
} jsonlite_token_block;

typedef struct jsonlite_token_pool_struct {
    jsonlite_token_block blocks[JSONLITE_TOKEN_POOL_FRONT];    
    uint8_t *content_pool;
    size_t content_pool_size;    
    jsonlite_token_pool_release_value_fn release_fn;
    
} jsonlite_token_pool_struct;

static void jsonlite_extend_capacity(jsonlite_token_pool pool, int index);
static int jsonlite_bucket_not_copied(jsonlite_token_pool pool, jsonlite_token_bucket *b);
static int jsonlite_token_compare(const uint8_t *t1, const uint8_t *t2, size_t length);
static uint32_t jsonlite_hash(const uint8_t *data, size_t len);
static jsonlite_token_bucket terminate_backet = {0, NULL, NULL, NULL, 0};

jsonlite_token_pool jsonlite_token_pool_create(jsonlite_token_pool_release_value_fn release_fn) {
    jsonlite_token_pool pool = (jsonlite_token_pool)malloc(sizeof(jsonlite_token_pool_struct));
    int i;
    for (i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        pool->blocks[i].buckets = &terminate_backet;
        pool->blocks[i].capacity = 0;
    }
    pool->release_fn = release_fn;
    pool->content_pool = NULL;
    pool->content_pool_size = 0;
    return pool;
}

void jsonlite_token_pool_copy_tokens(jsonlite_token_pool pool) {
    jsonlite_token_bucket *bucket;
    size_t size = pool->content_pool_size;
    int i;

    for (i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        bucket = pool->blocks[i].buckets;
        if (jsonlite_bucket_not_copied(pool, bucket)) {
            size += bucket->end - bucket->start;
        }
    }
    
    if (size == pool->content_pool_size) {
        return;
    }
    
	uint8_t *buffer = (uint8_t *)malloc(size);
    ptrdiff_t offset = 0;
    if (pool->content_pool != NULL) {
        offset = buffer - pool->content_pool;
        memcpy(buffer, pool->content_pool, pool->content_pool_size); // LCOV_EXCL_LINE
    }
    
    uint8_t *p = buffer + pool->content_pool_size;    
    for (i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        bucket = pool->blocks[i].buckets;
        if (bucket->start == NULL) {
            continue;
        }
        
        if (jsonlite_bucket_not_copied(pool, bucket)) {
            size_t length = bucket->end - bucket->start;
            memcpy(p, bucket->start, length); // LCOV_EXCL_LINE
            bucket->start = p,
            bucket->end = p + length,
            p += length;
        } else {
            bucket->start += offset;
            bucket->end += offset;
        }
    }
    
    free(pool->content_pool);
    pool->content_pool = buffer;
    pool->content_pool_size = size;
}

void jsonlite_token_pool_release(jsonlite_token_pool pool) {
    int i;
    if (pool == NULL) {
        return;
    }

    for (i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        jsonlite_token_bucket *bucket = pool->blocks[i].buckets;
        if (bucket->start == NULL) {
            continue;
        }
        
        if (pool->release_fn != NULL) {
            for (; bucket->start != NULL; bucket++) {
                pool->release_fn((void *)bucket->value);           
            }
        }

        free(pool->blocks[i].buckets);
    }
    
    free(pool->content_pool);
    free(pool);
}

jsonlite_token_bucket* jsonlite_token_pool_get_bucket(jsonlite_token_pool pool, jsonlite_token *token) {
    if (pool == NULL || token == NULL) {
        return NULL;
    }
    
    if (token->start == NULL || token->end == NULL) {
        return NULL;
    }
    
    size_t length = token->end - token->start;
    uint32_t hash = jsonlite_hash(token->start, length);
    uint32_t index = hash & JSONLITE_TOKEN_POOL_FRONT_MASK;
    jsonlite_token_bucket *bucket = pool->blocks[index].buckets;
    size_t capacity = pool->blocks[index].capacity;
    size_t count = 0;
    for (; bucket->start != NULL; count++, bucket++) {
        if (bucket->hash != hash) {
            continue;
        }
        
        if (length != bucket->end - bucket->start) {
            continue;
        }
        
        if (jsonlite_token_compare(token->start, bucket->start, length)) {
            return bucket;
        }
    }

    if (count >= capacity) {
        jsonlite_extend_capacity(pool, index);
    }
    
    bucket = pool->blocks[index].buckets + count;
    bucket->hash = hash;
    bucket->start = token->start;
    bucket->end = token->end;
    bucket->value = NULL;
    bucket[1].start = NULL;
    return bucket;
}

static int jsonlite_token_compare(const uint8_t *t1, const uint8_t *t2, size_t length) {
    return memcmp(t1, t2, length) == 0 ? 1 : 0;
}

static void jsonlite_extend_capacity(jsonlite_token_pool pool, int index) {
    size_t capacity = pool->blocks[index].capacity;
    if (capacity == 0) {
        capacity = 0x10;
    }
    
    size_t size = capacity * sizeof(jsonlite_token_bucket);
    jsonlite_token_bucket *buckets = pool->blocks[index].buckets;
	jsonlite_token_bucket *extended = (jsonlite_token_bucket *)malloc(2 * size);
    
    if (buckets->start != NULL) {
        memcpy(extended, buckets, size); // LCOV_EXCL_LINE
        free(buckets);
    }
    
    pool->blocks[index].buckets = extended;
    pool->blocks[index].capacity = 2 * capacity;
}

static int jsonlite_bucket_not_copied(jsonlite_token_pool pool, jsonlite_token_bucket *bucket) {
    if (bucket->start == NULL) {
        return 0;
    }
    
    int res = bucket->start < pool->content_pool;
    res |= bucket->start >= pool->content_pool + pool->content_pool_size;
    return res;
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

static uint32_t MurmurHash2 ( const void * key, int len)
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
    
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    
    // Initialize the hash to a 'random' value
    
    uint32_t h = len;
    
    // Mix 4 bytes at a time into the hash
    
    const unsigned char * data = (const unsigned char *)key;
    
    while(len >= 4)
    {
        uint32_t k = *(uint32_t*)data;
        
        k *= m;
        k ^= k >> r;
        k *= m;
        
        h *= m;
        h ^= k;
        
        data += 4;
        len -= 4;
    }
    
    // Handle the last few bytes of the input array
    
    switch(len)
    {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
            h *= m;
    };
    
    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    
    return h;
}

//-----------------------------------------------------------------------------

static uint32_t jsonlite_hash(const uint8_t *data, size_t len) {
    return MurmurHash2(data, (int)len);
}
