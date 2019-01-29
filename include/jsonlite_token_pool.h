#ifndef JSONLITE_TOKEN_POOL_H
#define JSONLITE_TOKEN_POOL_H

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_token.h"
#endif

#define JSONLITE_TOKEN_POOL_FRONT 0x80
#define JSONLITE_TOKEN_POOL_FRONT_MASK (JSONLITE_TOKEN_POOL_FRONT - 1)

typedef void (*jsonlite_token_pool_release_value_fn)(void *);
typedef struct jsonlite_token_pool_struct *jsonlite_token_pool;

typedef struct jsonlite_token_bucket {
    const uint8_t *start;
    const uint8_t *end;

    uint32_t token_hash;
    ptrdiff_t value_hash;

    const void *value;
} jsonlite_token_bucket;

typedef struct jsonlite_token_block {
    jsonlite_token_bucket *buckets;
    size_t capacity;
} jsonlite_token_block;

typedef struct jsonlite_token_pool_struct {
    jsonlite_token_block blocks[JSONLITE_TOKEN_POOL_FRONT];
    uint8_t *content_pool;
    size_t content_pool_size;
} jsonlite_token_pool_struct;

#define jsonlite_token_pool_estimate_size(count) ((count) * sizeof(jsonlite_token_pool_struct))

size_t jsonlite_token_pool_batch_init(void *mem, size_t size, jsonlite_token_pool *pools);
void jsonlite_token_pool_copy_tokens(jsonlite_token_pool pool);
void jsonlite_token_pool_batch_cleanup(jsonlite_token_pool *pools, size_t count, jsonlite_token_pool_release_value_fn release);
jsonlite_token_bucket *jsonlite_token_pool_get_bucket(jsonlite_token_pool pool, jsonlite_token *token);
uint32_t jsonlite_hash(const void *data, size_t len);

#endif
