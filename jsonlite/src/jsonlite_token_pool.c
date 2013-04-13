//
//  Copyright 2012, Andrii Mamchur
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

#include "../include/jsonlite_token_pool.h"
#include "../include/jsonlite_hash.h"
#include <stdlib.h>
#include <string.h>

typedef struct content_pool_size {
    jsonlite_token_bucket *buckets[JSONLITE_TOKEN_POOL_FRONT];
    size_t buckets_length[JSONLITE_TOKEN_POOL_FRONT];
    size_t buckets_capacity[JSONLITE_TOKEN_POOL_FRONT];
    
    uint8_t *content_pool;
    size_t content_pool_size;
    
    jsonlite_token_pool_release_value_fn release_fn;
    
} jsonlite_token_pool_struct;

static void jsonlite_extend_capacity(jsonlite_token_pool pool, int index);
static int jsonlite_bucket_not_copied(jsonlite_token_pool pool, jsonlite_token_bucket *b);
static int jsonlite_token_compare(const uint8_t *t1, const uint8_t *t2, size_t length);

jsonlite_token_pool jsonlite_token_pool_create(jsonlite_token_pool_release_value_fn release_fn) {
    jsonlite_token_pool pool = (jsonlite_token_pool)calloc(1, sizeof(jsonlite_token_pool_struct));
    pool->release_fn = release_fn;
    return pool;
}

void jsonlite_token_pool_copy_tokens(jsonlite_token_pool pool) {
    size_t size = pool->content_pool_size;
    uint8_t *buffer, *p;
    ptrdiff_t offset = 0, length;
    jsonlite_token_bucket *b;
	int i;

    for (i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        b = pool->buckets[i];
        if (jsonlite_bucket_not_copied(pool, b)) {
            size += b->end - b->start;
        }
    }
    
    if (size == pool->content_pool_size) {
        return;
    }
    
	buffer = (uint8_t *)malloc(size);
    if (pool->content_pool != NULL) {
        offset = buffer - pool->content_pool;
        memcpy(buffer, pool->content_pool, pool->content_pool_size);
    }
    p = buffer + pool->content_pool_size;
    
    for (i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        b = pool->buckets[i];
        if (b == NULL) {
            continue;
        }
        
        if (jsonlite_bucket_not_copied(pool, b)) {
            length = b->end - b->start;
            memcpy(p, b->start, length);
            b->start = p,
            b->end = p + length,
            p += length;
        } else {
            b->start += offset;
            b->end += offset;
        }
    }
    
    free(pool->content_pool);
    pool->content_pool = buffer;
    pool->content_pool_size = size;
}

void jsonlite_token_pool_release(jsonlite_token_pool pool) {
	ptrdiff_t i, j, count;
    jsonlite_token_bucket *bucket;
    
    if (pool == NULL) {
        return;
    }

    for (i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        bucket = pool->buckets[i];
        if (bucket == NULL) {
            continue;
        }
        
        if (pool->release_fn != NULL) {
            count = pool->buckets_length[i];            
            for (j = 0; j < count; j++, bucket++) {
                if (pool->release_fn != NULL) {
                    pool->release_fn((void *)bucket->value);
                }            
            }
        }

        free(pool->buckets[i]);
    }
    
    free(pool->content_pool);
    free(pool);
}

jsonlite_token_bucket* jsonlite_token_pool_get_bucket(jsonlite_token_pool pool, jsonlite_token *token) {
    size_t length, count;
    uint32_t hash, index;
    jsonlite_token_bucket *bucket;
    
    if (pool == NULL || token == NULL) {
        return NULL;
    }
    
    if (token->start == NULL || token->end == NULL) {
        return NULL;
    }
    
    length = token->end - token->start;
    hash = jsonlite_hash(token->start, length);
    index = hash & JSONLITE_TOKEN_POOL_FRONT_MASK;
    bucket = pool->buckets[index];
    count = pool->buckets_length[index];
    for (; count > 0; count--, bucket++) {
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

    if (pool->buckets_length[index] >= pool->buckets_capacity[index]) {
        jsonlite_extend_capacity(pool, index);
    }
    
    bucket = pool->buckets[index] + pool->buckets_length[index]++;
    bucket->hash = hash;
    bucket->start = token->start;
    bucket->end = token->end;
    bucket->value = NULL;
    return bucket;
}

static int jsonlite_token_compare(const uint8_t *t1, const uint8_t *t2, size_t length) {
    div_t d = div((int)length, sizeof(uint32_t));
    const uint32_t *a = (const uint32_t *)t1;
    const uint32_t *b = (const uint32_t *)t2;
	int i;

    for (i = 0; i < d.quot; i++) {
        if (*a++ != *b++) {
            return 0;
        }
    }
    
    t1 = (const uint8_t *)a;
    t2 = (const uint8_t *)b;
    
    switch (d.rem) {
        case 3: if (*t1++ != *t2++) return 0;
        case 2: if (*t1++ != *t2++) return 0;
        case 1: if (*t1++ != *t2++) return 0;
            break;
    }
    
    return 1;
}

static void jsonlite_extend_capacity(jsonlite_token_pool pool, int index) {
    jsonlite_token_bucket *b, *extended;
    size_t capacity, size;
    
    capacity = pool->buckets_capacity[index];
    if (capacity == 0) {
        capacity = 0x10;
    }
    
    size = capacity * sizeof(jsonlite_token_bucket);
    b = pool->buckets[index];
	extended = (jsonlite_token_bucket *)malloc(2 * size);
    
    if (b != NULL) {
        memcpy(extended, b, size);
        free(b);
    }
    
    pool->buckets[index] = extended;
    pool->buckets_capacity[index] = 2 * capacity;
}

static int jsonlite_bucket_not_copied(jsonlite_token_pool pool, jsonlite_token_bucket *b) {
    if (b == NULL) {
        return 0;
    }
    return b->start < pool->content_pool || b->start >= pool->content_pool + pool->content_pool_size;
}
