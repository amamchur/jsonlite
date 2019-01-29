#include "callback_recorder.hpp"

#include <gtest/gtest.h>
#include <jsonlite.h>
#include <string>

TEST(token_pool, should_be_initialized_from_memory) {
    uint8_t pools_mem[jsonlite_token_pool_estimate_size(3)];
    jsonlite_token_pool pools[3] = {nullptr};
    size_t count = jsonlite_token_pool_batch_init(pools_mem, sizeof(pools_mem), pools);
    EXPECT_EQ(count, 3);
    EXPECT_NE(pools[0], nullptr);
    EXPECT_NE(pools[1], nullptr);
    EXPECT_NE(pools[2], nullptr);

    jsonlite_token_pool_batch_cleanup(pools, count, nullptr);
}

TEST(token_pool, should_create_and_return_backet) {
    uint8_t pools_mem[jsonlite_token_pool_estimate_size(1)];
    jsonlite_token_pool pools[1] = {nullptr};
    size_t count = jsonlite_token_pool_batch_init(pools_mem, sizeof(pools_mem), pools);
    EXPECT_EQ(count, 1);

    jsonlite_token token;
    token.start = (uint8_t *)"test";
    token.end = token.start + strlen((char *)token.start);

    auto b1 = jsonlite_token_pool_get_bucket(pools[0], &token);
    EXPECT_NE(b1, nullptr);
    EXPECT_EQ(b1->value, nullptr);

    b1->value = malloc(5);
    ((char *)b1->value)[0] = 0;

    auto b2 = jsonlite_token_pool_get_bucket(pools[0], &token);
    EXPECT_NE(b2, nullptr);
    EXPECT_EQ(b2, b1);
    EXPECT_EQ(b2->value, b1->value);

    jsonlite_token_pool_batch_cleanup(pools, count, free);
}

TEST(token_pool, should_return_backet_without_hash_collision) {
    uint8_t pools_mem[jsonlite_token_pool_estimate_size(1)];
    jsonlite_token_pool pools[1] = {nullptr};
    size_t count = jsonlite_token_pool_batch_init(pools_mem, sizeof(pools_mem), pools);
    EXPECT_EQ(count, 1);

    jsonlite_token token1;
    token1.start = (uint8_t *)"collision_a";
    token1.end = token1.start + strlen((char *)token1.start);

    auto b1 = jsonlite_token_pool_get_bucket(pools[0], &token1);
    EXPECT_NE(b1, nullptr);
    EXPECT_EQ(b1->value, nullptr);
    EXPECT_EQ(b1->token_hash, 3013493526);

    jsonlite_token token2;
    token2.start = (uint8_t *)"collision_b";
    token2.end = token2.start + strlen((char *)token2.start);

    auto b2 = jsonlite_token_pool_get_bucket(pools[0], &token2);
    EXPECT_NE(b2, nullptr);
    EXPECT_NE(b2, b1);
    EXPECT_EQ(b2->value, nullptr);
    EXPECT_EQ(b2->token_hash, 3637409007);

    jsonlite_token token3;
    token3.start = (uint8_t *)"collision_c";
    token3.end = token3.start + strlen((char *)token3.start);

    auto b3 = jsonlite_token_pool_get_bucket(pools[0], &token3);
    EXPECT_NE(b3, nullptr);
    EXPECT_NE(b3, b1);
    EXPECT_NE(b3, b2);
    EXPECT_EQ(b3->value, nullptr);
    EXPECT_EQ(b3->token_hash, 4228115900);

    jsonlite_token_pool_batch_cleanup(pools, count, free);
}

TEST(token_pool, should_return_backet_with_hash_collision_same_size) {
    uint8_t pools_mem[jsonlite_token_pool_estimate_size(1)];
    jsonlite_token_pool pools[1] = {nullptr};
    size_t count = jsonlite_token_pool_batch_init(pools_mem, sizeof(pools_mem), pools);
    EXPECT_EQ(count, 1);

    jsonlite_token token1;
    token1.start = (uint8_t *)"collision_a";
    token1.end = token1.start + strlen((char *)token1.start);

    auto b1 = jsonlite_token_pool_get_bucket(pools[0], &token1);
    EXPECT_NE(b1, nullptr);
    EXPECT_EQ(b1->value, nullptr);
    EXPECT_EQ(b1->token_hash, 3013493526);

    jsonlite_token token2;
    token2.start = (uint8_t *)"collision_00P4PM60N0000000";
    token2.end = token2.start + strlen((char *)token2.start);

    auto b2 = jsonlite_token_pool_get_bucket(pools[0], &token2);
    EXPECT_NE(b2, nullptr);
    EXPECT_NE(b2, b1);
    EXPECT_EQ(b2->value, nullptr);
    EXPECT_EQ(b2->token_hash, 3013493526);

    jsonlite_token token3;
    token3.start = (uint8_t *)"collision_1NMMNK0O20000000";
    token3.end = token3.start + strlen((char *)token3.start);

    auto b3 = jsonlite_token_pool_get_bucket(pools[0], &token3);
    EXPECT_NE(b3, nullptr);
    EXPECT_NE(b3, b1);
    EXPECT_NE(b3, b2);
    EXPECT_EQ(b3->value, nullptr);
    EXPECT_EQ(b3->token_hash, 3013493526);

    jsonlite_token_pool_batch_cleanup(pools, count, nullptr);
}

typedef struct reusable_value {
    char *text;
    jsonlite_token_type type;
} reusable_value;

typedef struct test_context {
    jsonlite_token_pool *pools;
    int created;
    int reused;
    int matched;
} test_context;

static void collect_token_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    auto tctx = reinterpret_cast<test_context *>(ctx->client_state);
    auto pools = tctx->pools;
    auto type = (jsonlite_token_type)(t->type & jsonlite_token_type_mask);
    jsonlite_token_bucket * bucket = nullptr;
    switch (type) {
        case jsonlite_token_key:
        case jsonlite_token_string:
            bucket = jsonlite_token_pool_get_bucket(pools[0], t);
            break;
        case jsonlite_token_number:
            bucket = jsonlite_token_pool_get_bucket(pools[1], t);
            break;
        default:
            return;
    }

    size_t size = jsonlite_token_size_of_uft8(t);
    char *tmp = (char *)malloc(size);
    size_t length = jsonlite_token_to_uft8(t, tmp);
    tmp[length] = 0;

    if (bucket->value == nullptr) {
        auto rv = (reusable_value *)malloc(sizeof(reusable_value));
        rv->text = tmp;
        rv->type = t->type;

        bucket->value = rv;
        tctx->created++;
    } else {
        auto rv = (reusable_value *)bucket->value;
        int m = strcmp(rv->text, tmp);
        tctx->reused++;
        if (m == 0) {
            tctx->matched++;
        }

        free(tmp);
    }
}

static void free_values(void *ptr) {
    auto rv = (reusable_value *)ptr;
    free(rv->text);
    free(rv);
}

TEST(token_pool, should_be_reusable) {
    uint8_t pools_mem[jsonlite_token_pool_estimate_size(3)];
    jsonlite_token_pool pools[3] = {nullptr};
    size_t count = jsonlite_token_pool_batch_init(pools_mem, sizeof(pools_mem), pools);

    auto data = read_file_content("/deserializer/object_matrix.json");
    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    test_context ctx;
    ctx.pools = pools;
    ctx.created = 0;
    ctx.reused = 0;
    ctx.matched = 0;

    p->callbacks.token_found = collect_token_callback;
    p->callbacks.context.client_state = &ctx;

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ(ctx.created, 10);
    EXPECT_EQ(ctx.reused, 25);
    EXPECT_EQ(ctx.matched, 25);
    EXPECT_EQ(ctx.created + ctx.reused, 35);

    for (int i = 0; i < count; i++) {
        jsonlite_token_pool_copy_tokens(pools[i]);
    }

    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    ctx.created = 0;
    ctx.reused = 0;
    ctx.matched = 0;
    p->callbacks.token_found = collect_token_callback;
    p->callbacks.context.client_state = &ctx;

    result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ(ctx.created, 0);
    EXPECT_EQ(ctx.reused, 35);
    EXPECT_EQ(ctx.matched, 35);
    EXPECT_EQ(ctx.created + ctx.reused, 35);

    jsonlite_token_pool_batch_cleanup(pools, count, free_values);
}

TEST(token_pool, should_be_reusable_generated) {
    uint8_t pools_mem[jsonlite_token_pool_estimate_size(3)];
    jsonlite_token_pool pools[3] = {nullptr};
    size_t count = jsonlite_token_pool_batch_init(pools_mem, sizeof(pools_mem), pools);

    auto data = read_file_content("/success/generated.json");
    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    test_context ctx;
    ctx.pools = pools;
    ctx.created = 0;
    ctx.reused = 0;
    ctx.matched = 0;

    p->callbacks.token_found = collect_token_callback;
    p->callbacks.context.client_state = &ctx;

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ(ctx.created, 9239);
    EXPECT_EQ(ctx.reused, 23041);
    EXPECT_EQ(ctx.matched, 23041);
    EXPECT_EQ(ctx.created + ctx.reused, 32280);

    for (int i = 0; i < count; i++) {
        jsonlite_token_pool_copy_tokens(pools[i]);
    }

    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    ctx.created = 0;
    ctx.reused = 0;
    ctx.matched = 0;
    p->callbacks.token_found = collect_token_callback;
    p->callbacks.context.client_state = &ctx;

    result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ(ctx.created, 0);
    EXPECT_EQ(ctx.reused, 32280);
    EXPECT_EQ(ctx.matched, 32280);
    EXPECT_EQ(ctx.created + ctx.reused, 32280);

    jsonlite_token_pool_batch_cleanup(pools, count, free_values);
}