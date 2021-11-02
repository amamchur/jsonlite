#define JSONLITE_AMALGAMATED
#include "jsonlite.h"

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdlib.h>
#include <string.h>

static int jsonlite_null_buffer_set_append(jsonlite_buffer buffer, const void *data, size_t length) {
    return length == 0 ? 0 : -1;
}

jsonlite_buffer jsonlite_null_buffer() {
    static struct jsonlite_buffer_struct jsonlite_null_buffer_struct = {
            NULL,
            0,
            0,
            &jsonlite_null_buffer_set_append,
            &jsonlite_null_buffer_set_append
    };
	return &jsonlite_null_buffer_struct;
}

const void *jsonlite_buffer_data(jsonlite_buffer_const buffer) {
    return buffer->mem;
}

size_t jsonlite_buffer_size(jsonlite_buffer_const buffer) {
    return buffer->size;
}

size_t jsonlite_buffer_capacity(jsonlite_buffer_const buffer) {
    return buffer->capacity;
}

int jsonlite_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    return buffer->set_mem(buffer, data, length);
}

int jsonlite_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    return buffer->append_mem(buffer, data, length);
}
#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdlib.h>
#include <string.h>

static int jsonlite_static_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    if (length > buffer->capacity) {
        return -1;
    }

    buffer->size = length;
    memcpy(buffer->mem, data, length);
    return 0;
}

static int jsonlite_static_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    size_t total_size = buffer->size + length;
    if (total_size > buffer->capacity) {
        return -1;
    }

    memcpy(buffer->mem + buffer->size, data, length);
    buffer->size = total_size;
    return 0;
}

jsonlite_buffer jsonlite_buffer_static_init(void *mem, size_t size) {
	struct jsonlite_buffer_struct *buffer;

    if (mem == NULL) {
        return NULL;
    }

    if (size < sizeof(jsonlite_buffer_struct)) {
        return NULL;
    }

    buffer = (struct jsonlite_buffer_struct *)mem;
    buffer->set_mem = &jsonlite_static_buffer_set_mem;
    buffer->append_mem = &jsonlite_static_buffer_append_mem;
    buffer->mem = (uint8_t *)mem + sizeof(jsonlite_buffer_struct);
    buffer->size = 0;
    buffer->capacity = size - sizeof(jsonlite_buffer_struct);
    return buffer;
}
#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_builder.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdio.h>
#include <string.h>

#define MIN_DEPTH 2

#define jsonlite_builder_check_depth()                                                                                                                         \
    do {                                                                                                                                                       \
        if (builder->state >= builder->limit) {                                                                                                                \
            return jsonlite_result_depth_limit;                                                                                                                \
        }                                                                                                                                                      \
    } while (0)

enum {
    jsonlite_accept_object_begin = 0x0001,
    jsonlite_accept_object_end = 0x0002,
    jsonlite_accept_array_begin = 0x0004,
    jsonlite_accept_array_end = 0x0008,
    jsonlite_accept_key = 0x0010,
    jsonlite_accept_string = 0x0020,
    jsonlite_accept_number = 0x0040,
    jsonlite_accept_boolean = 0x0080,
    jsonlite_accept_null = 0x0100,
    jsonlite_accept_values_only = 0x0200,
    jsonlite_accept_next = 0x0400,

    jsonlite_accept_value = 0 | jsonlite_accept_object_begin | jsonlite_accept_array_begin | jsonlite_accept_string | jsonlite_accept_number |
                            jsonlite_accept_boolean | jsonlite_accept_null,
    jsonlite_accept_continue_object = 0 | jsonlite_accept_next | jsonlite_accept_key | jsonlite_accept_object_end,
    jsonlite_accept_continue_array = 0 | jsonlite_accept_next | jsonlite_accept_values_only | jsonlite_accept_value | jsonlite_accept_array_end
};

static int jsonlite_builder_accept(jsonlite_builder builder, jsonlite_write_state a);
static void jsonlite_builder_pop_state(jsonlite_builder builder);
static void jsonlite_builder_prepare_value_writing(jsonlite_builder builder);
static void jsonlite_builder_raw_char(jsonlite_builder builder, char data);
static void jsonlite_builder_write_uft8(jsonlite_builder builder, const void *data, size_t length);
static void jsonlite_builder_raw(jsonlite_builder builder, const void *data, size_t length);
static void jsonlite_builder_repeat(jsonlite_builder builder, char ch, size_t count);
static void jsonlite_builder_write_base64(jsonlite_builder builder, const void *data, size_t length);
static jsonlite_builder jsonlite_builder_configure(void *memory, size_t size, jsonlite_stream stream);

jsonlite_builder jsonlite_builder_init(void *memory, size_t size, jsonlite_stream stream) {
    if (size < jsonlite_builder_estimate_size(MIN_DEPTH)) {
        return NULL;
    }

    return jsonlite_builder_configure(memory, size, stream);
}

static jsonlite_builder jsonlite_builder_configure(void *memory, size_t size, jsonlite_stream stream) {
	jsonlite_builder builder = (jsonlite_builder)memory;
	size_t depth = (size - sizeof(jsonlite_builder_struct)) / sizeof(jsonlite_write_state);
    builder->state = (jsonlite_write_state *)((uint8_t *)builder + sizeof(jsonlite_builder_struct));
    builder->limit = builder->state + depth - 1;
    builder->stack = builder->state;
    builder->stream = stream;
    builder->indentation = 0;
    *builder->state = jsonlite_accept_object_begin | jsonlite_accept_array_begin;
    jsonlite_builder_set_double_format(builder, "%.16g");

    return builder;
}

void jsonlite_builder_set_indentation(jsonlite_builder builder, size_t indentation) {
    builder->indentation = indentation;
}

void jsonlite_builder_set_double_format(jsonlite_builder builder, const char *format) {
    strcpy(builder->doubleFormat, format);
}

static int jsonlite_builder_accept(jsonlite_builder builder, jsonlite_write_state a) {
    return (*builder->state & a) == a;
}

static void jsonlite_builder_pop_state(jsonlite_builder builder) {
    jsonlite_write_state *ws;
    ws = --builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
        *ws = jsonlite_accept_continue_array;
    } else {
        *ws = jsonlite_accept_continue_object;
    }
}

static void jsonlite_builder_prepare_value_writing(jsonlite_builder builder) {
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
        if (jsonlite_builder_accept(builder, jsonlite_accept_next)) {
            jsonlite_builder_raw_char(builder, ',');
        }

        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\n');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }
    } else {
        *ws &= ~jsonlite_accept_value;
        *ws |= jsonlite_accept_key;
    }
    *ws |= jsonlite_accept_next;
}

jsonlite_result jsonlite_builder_object_begin(jsonlite_builder builder) {
    jsonlite_builder_check_depth();

    if (jsonlite_builder_accept(builder, jsonlite_accept_object_begin)) {
        jsonlite_builder_prepare_value_writing(builder);
        *++builder->state = jsonlite_accept_object_end | jsonlite_accept_key;
        jsonlite_builder_raw_char(builder, '{');
        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_object_end(jsonlite_builder builder) {
    if (jsonlite_builder_accept(builder, jsonlite_accept_object_end)) {
        jsonlite_builder_pop_state(builder);
        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\n');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }
        jsonlite_builder_raw_char(builder, '}');
        return jsonlite_result_ok;
    }
    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_array_begin(jsonlite_builder builder) {
    jsonlite_builder_check_depth();

    if (jsonlite_builder_accept(builder, jsonlite_accept_array_begin)) {
        jsonlite_builder_prepare_value_writing(builder);
        *++builder->state = jsonlite_accept_array_end | jsonlite_accept_value | jsonlite_accept_values_only;
        jsonlite_builder_raw_char(builder, '[');
        return jsonlite_result_ok;
    }
    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_array_end(jsonlite_builder builder) {
    if (jsonlite_builder_accept(builder, jsonlite_accept_array_end)) {
        jsonlite_builder_pop_state(builder);
        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\n');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }
        jsonlite_builder_raw_char(builder, ']');
        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

static void jsonlite_builder_write_uft8(jsonlite_builder builder, const void *data, size_t length) {
    char b[2] = {'\\', '?'};
    const char *c = (const char *)data;
    const char *p = (const char *)data;
    const char *l = (const char *)data + length;

    jsonlite_builder_raw_char(builder, '\"');
next:
    if (c == l)
        goto end;
    switch (*c) {
    case '"':
        b[1] = '"';
        goto flush;
    case '\\':
        b[1] = '\\';
        goto flush;
    case '\b':
        b[1] = 'b';
        goto flush;
    case '\f':
        b[1] = 'f';
        goto flush;
    case '\n':
        b[1] = 'n';
        goto flush;
    case '\r':
        b[1] = 'r';
        goto flush;
    case '\t':
        b[1] = 't';
        goto flush;
    default:
        c++;
        goto next;
    }
flush:
    jsonlite_stream_write(builder->stream, p, c - p);
    jsonlite_stream_write(builder->stream, b, 2);
    p = ++c;
    goto next;
end:
    jsonlite_stream_write(builder->stream, p, c - p);
    jsonlite_builder_raw_char(builder, '\"');
}

jsonlite_result jsonlite_builder_key(jsonlite_builder builder, const void *utf8, size_t length) {
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_key)) {
        if (jsonlite_builder_accept(builder, jsonlite_accept_next)) {
            jsonlite_builder_raw_char(builder, ',');
        }

        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\n');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }

        jsonlite_builder_write_uft8(builder, utf8, length);
        if (builder->indentation != 0) {
            jsonlite_builder_raw(builder, ": ", 2);
        } else {
            jsonlite_builder_raw_char(builder, ':');
        }
        *ws = jsonlite_accept_value;

        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_string(jsonlite_builder builder, const void *utf8, size_t length) {
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_value)) {
        jsonlite_builder_prepare_value_writing(builder);
        jsonlite_builder_write_uft8(builder, utf8, length);
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
            *ws = jsonlite_accept_continue_array;
        } else {
            *ws = jsonlite_accept_continue_object;
        }
        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_int(jsonlite_builder builder, long long value) {
    char buff[64];
    size_t size = 0;
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_value)) {
        jsonlite_builder_prepare_value_writing(builder);
        size = (size_t)sprintf(buff, "%lld", value);
        jsonlite_builder_raw(builder, buff, size);
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
            *ws = jsonlite_accept_continue_array;
        } else {
            *ws = jsonlite_accept_continue_object;
        }
        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_double(jsonlite_builder builder, double value) {
    char buff[64];
    size_t size = 0;
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_value)) {
        jsonlite_builder_prepare_value_writing(builder);
        size = (size_t)sprintf(buff, builder->doubleFormat, value);
        jsonlite_builder_raw(builder, buff, size);
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
            *ws = jsonlite_accept_continue_array;
        } else {
            *ws = jsonlite_accept_continue_object;
        }
        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_true(jsonlite_builder builder) {
    static const char value[] = "true";
    jsonlite_write_state *ws = builder->state;
    if (!(jsonlite_builder_accept(builder, jsonlite_accept_value))) {
        return jsonlite_result_not_allowed;
    }

    jsonlite_builder_prepare_value_writing(builder);
    jsonlite_builder_raw(builder, (char *)value, sizeof(value) - 1);
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
        *ws = jsonlite_accept_continue_array;
    } else {
        *ws = jsonlite_accept_continue_object;
    }

    return jsonlite_result_ok;
}

jsonlite_result jsonlite_builder_false(jsonlite_builder builder) {
    static const char value[] = "false";
    jsonlite_write_state *ws = builder->state;
    if (!(jsonlite_builder_accept(builder, jsonlite_accept_value))) {
        return jsonlite_result_not_allowed;
    }

    jsonlite_builder_prepare_value_writing(builder);
    jsonlite_builder_raw(builder, (char *)value, sizeof(value) - 1);
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
        *ws = jsonlite_accept_continue_array;
    } else {
        *ws = jsonlite_accept_continue_object;
    }

    return jsonlite_result_ok;
}

jsonlite_result jsonlite_builder_null(jsonlite_builder builder) {
    static const char value[] = "null";
    jsonlite_write_state *ws = builder->state;
    if (!(jsonlite_builder_accept(builder, jsonlite_accept_value))) {
        return jsonlite_result_not_allowed;
    }

    jsonlite_builder_prepare_value_writing(builder);
    jsonlite_builder_raw(builder, (char *)value, sizeof(value) - 1);
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
        *ws = jsonlite_accept_continue_array;
    } else {
        *ws = jsonlite_accept_continue_object;
    }

    return jsonlite_result_ok;
}

static void jsonlite_builder_raw(jsonlite_builder builder, const void *data, size_t length) {
    jsonlite_stream_write(builder->stream, data, length);
}

static void jsonlite_builder_repeat(jsonlite_builder builder, char ch, size_t count) {
    size_t i = 0;
    for (; i < count; i++) {
        jsonlite_stream_write(builder->stream, &ch, 1);
    }
}

static void jsonlite_builder_raw_char(jsonlite_builder builder, char data) {
    jsonlite_stream_write(builder->stream, &data, 1);
}

jsonlite_result jsonlite_builder_raw_key(jsonlite_builder builder, const void *data, size_t length) {
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_key)) {
        if (jsonlite_builder_accept(builder, jsonlite_accept_next)) {
            jsonlite_builder_raw(builder, ",", 1);
        }

        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\n');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }

        jsonlite_builder_raw_char(builder, '\"');
        jsonlite_builder_raw(builder, data, length);
        jsonlite_builder_raw_char(builder, '\"');
        if (builder->indentation != 0) {
            jsonlite_builder_raw(builder, ": ", 2);
        } else {
            jsonlite_builder_raw_char(builder, ':');
        }
        *ws = jsonlite_accept_value;

        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_raw_string(jsonlite_builder builder, const void *data, size_t length) {
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_value)) {
        jsonlite_builder_prepare_value_writing(builder);
        jsonlite_builder_raw_char(builder, '\"');
        jsonlite_builder_raw(builder, data, length);
        jsonlite_builder_raw_char(builder, '\"');
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
            *ws = jsonlite_accept_continue_array;
        } else {
            *ws = jsonlite_accept_continue_object;
        }

        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_raw_value(jsonlite_builder builder, const void *data, size_t length) {
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_value)) {
        jsonlite_builder_prepare_value_writing(builder);
        jsonlite_builder_raw(builder, data, length);
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
            *ws = jsonlite_accept_continue_array;
        } else {
            *ws = jsonlite_accept_continue_object;
        }

        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

static void jsonlite_builder_write_base64(jsonlite_builder builder, const void *data, size_t length) {
    static const char encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char buffer[5] = {0};
    const uint8_t *c = (const uint8_t *)data;
    const uint8_t *l = c + length;
    uint32_t bits;
    jsonlite_stream_write(builder->stream, "\"", 1);
next:
    switch (l - c) {
    case 0:
        goto done;
    case 1:
        bits = (uint32_t)(*c++) << 16;
        buffer[0] = encode[(bits & 0x00FC0000) >> 18];
        buffer[1] = encode[(bits & 0x0003F000) >> 12];
        buffer[2] = '=';
        buffer[3] = '=';
        l = c;
        goto write;
    case 2:
        bits = (uint32_t)(*c++) << 16;
        bits |= (uint32_t)(*c++ << 8);
        buffer[0] = encode[(bits & 0x00FC0000) >> 18];
        buffer[1] = encode[(bits & 0x0003F000) >> 12];
        buffer[2] = encode[(bits & 0x00000FC0) >> 6];
        buffer[3] = '=';
        l = c;
        goto write;
    default:
        bits = (uint32_t)(*c++) << 16;
        bits |= *c++ << 8;
        bits |= *c++;
        buffer[0] = encode[(bits & 0x00FC0000) >> 18];
        buffer[1] = encode[(bits & 0x0003F000) >> 12];
        buffer[2] = encode[(bits & 0x00000FC0) >> 6];
        buffer[3] = encode[(bits & 0x0000003F)];
        goto write;
    }
write:
    jsonlite_stream_write(builder->stream, buffer, 4);
    goto next;
done:
    jsonlite_stream_write(builder->stream, "\"", 1);

}

jsonlite_result jsonlite_builder_base64_value(jsonlite_builder builder, const void *data, size_t length) {
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_value)) {
        jsonlite_builder_prepare_value_writing(builder);
        jsonlite_builder_write_base64(builder, data, length);
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only)) {
            *ws = jsonlite_accept_continue_array;
        } else {
            *ws = jsonlite_accept_continue_object;
        }

        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}
#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_parser.h"
#include "jsonlite_utf.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdlib.h>
#include <string.h>

#define MIN_DEPTH 2
#define CALL_VALUE_CALLBACK(cbs, type, token)   (cbs.token_found(&cbs.context, token))
#define CALL_STATE_CALLBACK(cbs, type, event)    (cbs.event_occurred(&cbs.context, event))

#define HEX_CHAR_TO_INT(c, step)        \
if (0x30 <= *c && *c <= 0x39) {         \
    hex_value = *c - 0x30;              \
} else if (0x41 <= *c && *c <= 0x46) {  \
    hex_value = *c - 0x37;              \
} else if (0x61 <= *c && *c <= 0x66) {  \
    hex_value = *c - 0x57;              \
} else goto error_escape;               \
c += step

#define CASE_NUMBER_TOKEN_END   \
case 0x09: goto number_parsed;  \
case 0x0A: goto number_parsed;  \
case 0x0D: goto number_parsed;  \
case 0x20: goto number_parsed;  \
case 0x2C: goto number_parsed;  \
case 0x5D: goto number_parsed;  \
case 0x7D: goto number_parsed

enum {
    state_start,
    state_object_key,
    state_object_key_only,
    state_object_key_or_end,
    state_object_colon,
    state_object_comma_or_end,
    state_array_value_or_end,
    state_array_comma_or_end,
    state_value,
    state_end,

    state_stop = 1 << 7
};

static void jsonlite_do_parse(jsonlite_parser parser);
static void empty_value_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
}

static void empty_state_callback(jsonlite_callback_context *ctx, jsonlite_event event) {
}

static jsonlite_parser jsonlite_parser_configure(void *memory, size_t size, jsonlite_buffer rest_buffer) {
    size_t depth = (size - sizeof(jsonlite_parser_struct)) / sizeof(parse_state);
    jsonlite_parser parser = (jsonlite_parser)memory;
    parser->result = jsonlite_result_unknown;
    parser->rest_buffer = rest_buffer;
    parser->control = NULL;
    parser->current = ((uint8_t *)parser + sizeof(jsonlite_parser_struct));
    parser->current[0] = state_end;
    parser->current[1] = state_start;
    parser->last = parser->current + depth;
    parser->current++;
    jsonlite_parser_callbacks_init(&parser->callbacks);

    return parser;
}

jsonlite_parser jsonlite_parser_init(void *memory, size_t size, jsonlite_buffer rest_buffer) {
    if (memory == NULL) {
        return NULL;
    }

    if (rest_buffer == NULL) {
        return NULL;
    }

    if (size < jsonlite_parser_estimate_size(MIN_DEPTH)) {
        return NULL;
    }

    return jsonlite_parser_configure(memory, size, rest_buffer);
}

void jsonlite_parser_set_callback(jsonlite_parser parser, const jsonlite_parser_callbacks *cbs) {
    parser->callbacks = *cbs;
    parser->callbacks.context.parser = parser;
}

jsonlite_result jsonlite_parser_get_result(jsonlite_parser parser) {
    return parser->result;
}

jsonlite_result jsonlite_parser_tokenize(jsonlite_parser parser, const void *buffer, size_t size) {
    size_t rest_size = jsonlite_buffer_size(parser->rest_buffer);
	if (rest_size > 0) {
        if (jsonlite_buffer_append_mem(parser->rest_buffer, buffer, size) < 0) {
            return jsonlite_result_out_of_memory;
        }

        parser->buffer = jsonlite_buffer_data(parser->rest_buffer);
        parser->cursor = parser->buffer;
        parser->limit = parser->buffer + jsonlite_buffer_size(parser->rest_buffer);
    } else {
        parser->buffer = buffer;
        parser->cursor = parser->buffer;
        parser->limit = parser->buffer + size;
    }

    jsonlite_do_parse(parser);

    return parser->result;
}

jsonlite_result jsonlite_parser_resume(jsonlite_parser parser) {
    if (parser->result != jsonlite_result_suspended) {
        return jsonlite_result_not_allowed;
    }

    jsonlite_do_parse(parser);
    return parser->result;
}

jsonlite_result jsonlite_parser_suspend(jsonlite_parser parser) {
    return jsonlite_parser_terminate(parser, jsonlite_result_suspended);
}

jsonlite_result jsonlite_parser_terminate(jsonlite_parser parser, jsonlite_result result) {
    if (parser->control == NULL) {
        return jsonlite_result_not_allowed;
    }

    parser->result = result;
    *parser->control |= state_stop;
    return jsonlite_result_ok;
}

void jsonlite_parser_callbacks_init(jsonlite_parser_callbacks *cbs) {
    cbs->event_occurred = &empty_state_callback;
    cbs->token_found = &empty_value_callback;
}

static void jsonlite_do_parse(jsonlite_parser parser) {
    const uint8_t *c = parser->cursor;
    const uint8_t *l = parser->limit;
    const uint8_t *token_start = NULL;
    const parse_state *last = parser->last;
    parse_state *state = parser->current;
    jsonlite_token token = {NULL, NULL, NULL, jsonlite_none};
    jsonlite_result result = jsonlite_result_ok;
    uint32_t value, utf32;
    uint8_t hex_value;
	int res;

    *state &= ~state_stop;
    parser->control = state;
    goto select_state;

structure_finished:
    if (*state == state_end)            goto end;
skip_char_and_whitespaces:
    c++;
select_state:
    if (c == l)     goto end_of_stream_whitespaces;
    if (*c == 0x20) goto skip_char_and_whitespaces;
    if (*c == 0x0A) goto skip_char_and_whitespaces;
    if (*c == 0x0D) goto skip_char_and_whitespaces;
    if (*c == 0x09) goto skip_char_and_whitespaces;
    token_start = c;

    switch (*state) {
        case state_value:               goto parse_value;
        case state_object_key:          goto parse_string;
        case state_object_key_only:     goto parse_object_key_only;
        case state_object_key_or_end:   goto parse_object_key_or_end;
        case state_object_colon:        goto parse_colon;
        case state_object_comma_or_end: goto parse_object_comma_end;
        case state_array_comma_or_end:  goto parse_array_comma_end;
        case state_array_value_or_end:  goto parse_array_value_end;
        case state_start:
            if (*c == 0x7B)             goto parse_object;
            if (*c == 0x5B)             goto parse_array_state;
            goto error_exp_ooa;
        case state_end:                 goto end;
        default:
            result = parser->result;
            goto end;
    }
parse_object:
    *state = state_object_key_or_end;
    CALL_STATE_CALLBACK(parser->callbacks, object_start, jsonlite_event_object_start);
    goto skip_char_and_whitespaces;
parse_array_state:
    *state = state_array_value_or_end;
    CALL_STATE_CALLBACK(parser->callbacks, array_start, jsonlite_event_array_start);
    goto skip_char_and_whitespaces;
parse_colon:
    if (*c != 0x3A) goto error_exp_colon;
    *state = state_object_comma_or_end;
    *++state = state_value;
    goto skip_char_and_whitespaces;
parse_object_comma_end:
    switch (*c) {
        case 0x2C:
            *state = state_object_key_only;
            goto skip_char_and_whitespaces;
        case 0x7D:
            state--;
            CALL_STATE_CALLBACK(parser->callbacks, object_end, jsonlite_event_object_end);
            goto structure_finished;
        default: goto error_exp_coe;
    }
parse_array_value_end:
    switch (*c) {
        case 0x5D:
            state--;
            CALL_STATE_CALLBACK(parser->callbacks, array_end, jsonlite_event_array_end);
            goto structure_finished;
        default:
            *state = state_array_comma_or_end;
            if (++state == last) goto error_depth;
            *state = state_value;
            goto parse_value;
    }
parse_array_comma_end:
    switch (*c) {
        case 0x2C:
            *++state = state_value;
            goto skip_char_and_whitespaces;
        case 0x5D:
            state--;
            CALL_STATE_CALLBACK(parser->callbacks, array_end, jsonlite_event_array_end);
            goto structure_finished;
        default: goto error_exp_coe;
    }
parse_value:
    if (0x31 <= *c && *c <= 0x39)   goto parse_digit_leading_number;
    if (*c == 0x30)                 goto parse_zero_leading_number;
    if (*c == 0x2D)                 goto parse_negative_number;
    if (*c == 0x22)                 goto parse_string;
    if (*c == 0x74)                 goto parse_true_token;
    if (*c == 0x66)                 goto parse_false_token;
    if (*c == 0x6E)                 goto parse_null_token;
    if (*c == 0x7B)                 goto parse_object;
    if (*c == 0x5B)                 goto parse_array_state;
    goto error_exp_value;

/* Number parsing */
parse_negative_number:
    token.start = c;
    token.type = jsonlite_token_number | jsonlite_number_int | jsonlite_number_negative;
    if (++c == l)                   goto end_of_stream;
    if (0x31 <= *c && *c <= 0x39)   goto parse_digits;
    if (*c == 0x30)                 goto parse_exponent_or_fraction;
    goto error_number;
parse_zero_leading_number:
    token.start = c;
    token.type = jsonlite_token_number | jsonlite_number_int | jsonlite_number_zero_leading;
parse_exponent_or_fraction:
    if (++c == l)                   goto end_of_stream;
    switch (*c) {
        CASE_NUMBER_TOKEN_END;
        case 0x2E:                  goto parse_fraction;
        case 0x45:                  goto parse_exponent;
        case 0x65:                  goto parse_exponent;
        default:                    goto error_number;
    }
parse_fraction:
    token.type |= jsonlite_number_frac;
    if (++c == l)                   goto end_of_stream;
    if (0x30 <= *c && *c <= 0x39)   goto parse_frac_number;
    goto error_number;
parse_frac_number:
    if (++c == l)                   goto end_of_stream;
    if (0x30 <= *c && *c <= 0x39)   goto parse_frac_number;
    switch (*c) {
        CASE_NUMBER_TOKEN_END;
        case 0x45:                  goto parse_exponent;
        case 0x65:                  goto parse_exponent;
        default:                    goto error_number;
    }
parse_exponent:
    token.type |= jsonlite_number_exp;
    if (++c == l)                   goto end_of_stream;
    if (0x30 <= *c && *c <= 0x39)   goto parse_exponent_number;
    switch(*c) {
        case 0x2B:                  goto parse_exponent_sign;
        case 0x2D:                  goto parse_exponent_sign;
        default:                    goto error_number;
    }
parse_exponent_sign:
    if (++c == l)                   goto end_of_stream;
    if (0x30 <= *c && *c <= 0x39)   goto parse_exponent_number;
    goto error_number;
parse_exponent_number:
    if (++c == l)                   goto end_of_stream;
    if (0x30 <= *c && *c <= 0x39)   goto parse_exponent_number;
    switch(*c) {
        CASE_NUMBER_TOKEN_END;
        default:                    goto error_number;
    }
parse_digit_leading_number:
    token.start = c;
    token.type = jsonlite_token_number | jsonlite_number_int | jsonlite_number_digit_leading;
parse_digits:
    if (++c == l)                   goto end_of_stream;
    if (0x30 <= *c && *c <= 0x39)   goto parse_digits;
    switch(*c) {
        CASE_NUMBER_TOKEN_END;
        case 0x2E:                  goto parse_fraction;
        case 0x45:                  goto parse_exponent;
        case 0x65:                  goto parse_exponent;
        default:                    goto error_number;
    }
number_parsed:
    token.end = c;
    state--;
    CALL_VALUE_CALLBACK(parser->callbacks, number_found, &token);
    goto select_state;

parse_object_key_or_end:
    switch (*c) {
        case 0x22:
            *state = state_object_colon;
            if (++state == last) goto error_depth;
            *state = state_object_key;
            goto parse_string;
        case 0x7D:
            state--;
            CALL_STATE_CALLBACK(parser->callbacks, object_end, jsonlite_event_object_end);
            goto structure_finished;
        default: goto error_exp_koe;
    }
parse_object_key_only:
    if (*c != 0x22) goto error_exp_key;
    *state = state_object_colon;
    *++state = state_object_key;

/* String parsing */
parse_string:
    token.type = jsonlite_string_ascii;
    token.start = c + 1;
next_char:
    if (++c == l)       goto end_of_stream;
    if (*c == 0x22)     goto string_parsed;
    if (*c == 0x5C)     goto escaped;
    if (*c > 0x7F)      goto utf8;
    if (*c < 0x20)      goto error_token;
    goto next_char;
escaped:
    token.type |= jsonlite_string_escape;
    if (++c == l)       goto end_of_stream;
    switch (*c) {
        case 0x22:      goto next_char;     /* "    quotation mark  U+0022 */
        case 0x2F:      goto next_char;     /* /    solidus         U+002F */
        case 0x5C:      goto next_char;     /* \    reverse solidus U+005C */
        case 0x62:      goto next_char;     /* b    backspace       U+0008 */
        case 0x66:      goto next_char;     /* f    form feed       U+000C */
        case 0x6E:      goto next_char;     /* n    line feed       U+000A */
        case 0x72:      goto next_char;     /* r    carriage return U+000D */
        case 0x74:      goto next_char;     /* t    tab             U+0009 */
        case 0x75:      goto hex;           /* uXXXX                U+XXXX */
        default:        goto error_escape;
    }
hex:
    token.type |= jsonlite_string_unicode_escape;
    if (c++ + 4 >= l)       goto end_of_stream;
    HEX_CHAR_TO_INT(c, 1);  value = hex_value;
    HEX_CHAR_TO_INT(c, 1);  value = (uint32_t)(value << 4) | hex_value;
    HEX_CHAR_TO_INT(c, 1);  value = (uint32_t)(value << 4) | hex_value;
    HEX_CHAR_TO_INT(c, 0);  value = (uint32_t)(value << 4) | hex_value;

    if ((value & 0xFFFFu) >= 0xFFFEu)           token.type |= jsonlite_string_unicode_noncharacter;
    if (value >= 0xFDD0u && value <= 0xFDEFu)   token.type |= jsonlite_string_unicode_noncharacter;
    if (0xD800 > value || value > 0xDBFF)       goto next_char;

    /* UTF-16 Surrogate */
    utf32 = (value - 0xD800) << 10;
    if (c++ + 6 >= l)       goto end_of_stream;
    if (*c++ != 0x5C)       goto error_escape;
    if (*c++ != 0x75)       goto error_escape;
    HEX_CHAR_TO_INT(c, 1);  value = hex_value;
    HEX_CHAR_TO_INT(c, 1);  value = (uint32_t)(value << 4) | hex_value;
    HEX_CHAR_TO_INT(c, 1);  value = (uint32_t)(value << 4) | hex_value;
    HEX_CHAR_TO_INT(c, 0);  value = (uint32_t)(value << 4) | hex_value;

    if (value < 0xDC00 || value > 0xDFFF)       goto error_escape;
    utf32 += value - 0xDC00 + 0x10000;
    if ((utf32 & 0x0FFFFu) >= 0x0FFFEu)         token.type |= jsonlite_string_unicode_noncharacter;
    goto next_char;
utf8:
    token.type |= jsonlite_string_utf8;
    /*
        See https://en.wikipedia.org/wiki/UTF-8#Description
        0xxxxxxx -> ASCII char, should not occur here, checked in next_char state
        10xxxxxx -> error
        110xxxxx -> sequence length 2
        1110xxxx -> sequence length 3
        11110xxx -> sequence length 4
        11111xxx -> error
     */
    res = jsonlite_utf8_sequence_length(*c);
    if (2 > res || res > 4) goto error_utf8;
    if (c + res >= l) goto end_of_stream;
    utf32 = (uint32_t)(*c & (0xFF >> (res)));
    value = 0xAAAAAAAA; /* == 1010... */
    switch (res) {
        case 4: value = (value << 2) | (*++c >> 6), utf32 = (utf32 << 6) | (*c & 0x3F);
        case 3: value = (value << 2) | (*++c >> 6), utf32 = (utf32 << 6) | (*c & 0x3F);
        default: value = (value << 2) | (*++c >> 6), utf32 = (utf32 << 6) | (*c & 0x3F);
            if (value != 0xAAAAAAAA)                    goto error_utf8;
            if ((utf32 & 0xFFFFu) >= 0xFFFEu)           {
                token.type |= jsonlite_string_unicode_noncharacter;
            }
            if (0xFDD0u <= utf32 && utf32 <= 0xFDEFu)   {
                token.type |= jsonlite_string_unicode_noncharacter;
            }
            break;
    }
    goto next_char;
string_parsed:
    token.end = c;
    parser->cursor = c + 1;
    if (*state-- == state_value) {
        token.type = (token.type & ~jsonlite_token_type_mask) | jsonlite_token_string;
        CALL_VALUE_CALLBACK(parser->callbacks, string_found, &token);
    } else {
        token.type = (token.type & ~jsonlite_token_type_mask) | jsonlite_token_key;
        CALL_VALUE_CALLBACK(parser->callbacks, key_found, &token);
    }
    goto skip_char_and_whitespaces;

/* Primitive tokens */
parse_true_token:
    if (c++ + 3 >= l)   goto end_of_stream;
    if (*c++ != 0x72)   goto error_token;
    if (*c++ != 0x75)   goto error_token;
    if (*c++ != 0x65)   goto error_token;
    state--;
    token.type = jsonlite_token_true;
    CALL_VALUE_CALLBACK(parser->callbacks, true_found,  &token);
    goto select_state;
parse_false_token:
    if (c++ + 4 >= l)   goto end_of_stream;
    if (*c++ != 0x61)   goto error_token;
    if (*c++ != 0x6C)   goto error_token;
    if (*c++ != 0x73)   goto error_token;
    if (*c++ != 0x65)   goto error_token;
    state--;
    token.type = jsonlite_token_false;
    CALL_VALUE_CALLBACK(parser->callbacks, false_found,  &token);
    goto select_state;
parse_null_token:
    if (c++ + 3 >= l)   goto end_of_stream;
    if (*c++ != 0x75)   goto error_token;
    if (*c++ != 0x6C)   goto error_token;
    if (*c++ != 0x6C)   goto error_token;
    state--;
    token.type = jsonlite_token_null;
    CALL_VALUE_CALLBACK(parser->callbacks, null_found,  &token);
    goto select_state;

/* Error states. */
error_depth:        result = jsonlite_result_depth_limit;               goto end;
error_exp_ooa:      result = jsonlite_result_expected_object_or_array;  goto end;
error_exp_value:    result = jsonlite_result_expected_value;            goto end;
error_exp_koe:      result = jsonlite_result_expected_key_or_end;       goto end;
error_exp_key:      result = jsonlite_result_expected_key;              goto end;
error_exp_colon:    result = jsonlite_result_expected_colon;            goto end;
error_exp_coe:      result = jsonlite_result_expected_comma_or_end;     goto end;
error_escape:       result = jsonlite_result_invalid_escape;            goto end;
error_number:       result = jsonlite_result_invalid_number;            goto end;
error_token:        result = jsonlite_result_invalid_token;             goto end;
error_utf8:         result = jsonlite_result_invalid_utf8;              goto end;

/* End of stream states. */
end_of_stream_whitespaces:
    token_start = l;
end_of_stream:
    result = jsonlite_result_end_of_stream;
end:
    parser->result = result;
    parser->current = state;
    parser->control = NULL;
    parser->cursor = c;
    parser->callbacks.event_occurred(&parser->callbacks.context, jsonlite_event_finished);
    if (result != jsonlite_result_end_of_stream) {
        return;
    }

    res = jsonlite_buffer_set_mem(parser->rest_buffer, token_start, parser->limit - token_start);
    if (res < 0) {
        parser->result = jsonlite_result_out_of_memory;
    } else {
        parser->buffer = jsonlite_buffer_data(parser->rest_buffer);
        parser->limit = parser->buffer + jsonlite_buffer_size(parser->rest_buffer);
    }
}
#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_stream.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    return stream->write(stream, data, length);
}

static int jsonlite_null_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    (void)(stream);
    (void)(data);
    return (int)length;
}

jsonlite_stream jsonlite_stream_null() {
    static struct jsonlite_stream_struct jsonlite_null_stream_struct = {jsonlite_null_stream_write};
    return &jsonlite_null_stream_struct;
}
#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_stream_static.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

size_t jsonlite_stream_static_written_bytes(jsonlite_stream stream) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    return mem_stream->written;
}

const void * jsonlite_stream_static_data(jsonlite_stream stream) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    return mem_stream->buffer;
}

static int jsonlite_static_mem_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    size_t write_limit = mem_stream->size - mem_stream->written;
    if (mem_stream->enabled && write_limit >= length) {
        memcpy(mem_stream->buffer + mem_stream->written, data, length);
        mem_stream->written += length;
    } else {
        mem_stream->enabled = 0;
        return 0;
    }

    return (int)length;
}

jsonlite_stream jsonlite_stream_static_init(void *buffer, size_t size) {
    struct jsonlite_stream_struct *stream;
	jsonlite_static_mem_stream *mem_stream;
	int extra_size = (int)size - sizeof(jsonlite_stream_struct) - sizeof(jsonlite_static_mem_stream);
    if (extra_size <= 0) {
        return NULL;
    }

    stream = (struct jsonlite_stream_struct *)buffer;
    stream->write = jsonlite_static_mem_stream_write;

    mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    mem_stream->buffer = (uint8_t *)mem_stream + sizeof(jsonlite_static_mem_stream);
    mem_stream->size = (size_t)extra_size;
    mem_stream->written = 0;
    mem_stream->enabled = 1;

    return stream;
}
#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_token.h"
#include "jsonlite_utf.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdlib.h>

static uint8_t jsonlite_hex_char_to_uint8(uint8_t c) {
    jsonlite_stack_check();
    if (c >= 'a') {
        return (uint8_t)(c - 'a' + 10);
    }

    if (c >= 'A') {
        return (uint8_t)(c - 'A' + 10);
    }

    return (uint8_t)(c - '0');
}

static int unicode_char_to_utf16(uint32_t utf32, uint16_t *utf16) {
    uint32_t v = utf32 - 0x10000;
    uint32_t vh = v >> 10;
    uint32_t vl = v & 0x3FF;
	if (utf32 <= 0xFFFF) {
        *utf16 = (uint16_t)utf32;
        return 1;
    }

    *utf16++ = (uint16_t)(0xD800 + vh);
    *utf16 = (uint16_t)(0xDC00 + vl);
    return 2;
}

size_t jsonlite_token_size_of_uft8(jsonlite_token *ts) {
    return (size_t)(ts->end - ts->start + 1);
}

size_t jsonlite_token_to_uft8(jsonlite_token *ts, void *buffer) {
    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
    uint32_t value, utf32;
  	uint8_t *c = (uint8_t *)buffer;
    int res;
step:
    if (p == l)         goto done;
    if (*p == '\\')     goto escaped;
    if (*p >= 0x80)     goto utf8;
    *c++ = *p++;
    goto step;
escaped:
    switch (*++p) {
        case 34:    *c++ = '"';     p++; goto step;
        case 47:    *c++ = '/';     p++; goto step;
        case 92:    *c++ = '\\';    p++; goto step;
        case 98:    *c++ = '\b';    p++; goto step;
        case 102:   *c++ = '\f';    p++; goto step;
        case 110:   *c++ = '\n';    p++; goto step;
        case 114:   *c++ = '\r';    p++; goto step;
        case 116:   *c++ = '\t';    p++; goto step;
        default:    break;
	}

    /* UTF-16 */
    p++;
    utf32 = jsonlite_hex_char_to_uint8(*p++);
    utf32 = (uint32_t)(utf32 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf32 = (uint32_t)(utf32 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf32 = (uint32_t)(utf32 << 4) | jsonlite_hex_char_to_uint8(*p++);
    if (0xD800 > utf32 || utf32 > 0xDBFF) goto encode;

    /* UTF-16 Surrogate */
    p += 2;
    utf32 = (utf32 - 0xD800) << 10;
    value = jsonlite_hex_char_to_uint8(*p++);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*p++);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*p++);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf32 += value - 0xDC00 + 0x10000;
encode:
    if (utf32 < 0x80) {
        *c++ = (uint8_t)utf32;
    } else if (utf32 < 0x0800) {
        c[1] = (uint8_t)((utf32 & 0x3F) | 0x80);
        utf32 = utf32 >> 6;
        c[0] = (uint8_t)(utf32 | 0xC0);
        c += 2;
    } else if (utf32 < 0x10000) {
        c[2] = (uint8_t)((utf32 & 0x3F) | 0x80);
        utf32 = utf32 >> 6;
        c[1] = (uint8_t)((utf32 & 0x3F) | 0x80);
        utf32 = utf32 >> 6;
        c[0] = (uint8_t)(utf32 | 0xE0);
        c += 3;
    } else {
        c[3] = (uint8_t)((utf32 & 0x3F) | 0x80);
        utf32 = utf32 >> 6;
        c[2] = (uint8_t)((utf32 & 0x3F) | 0x80);
        utf32 = utf32 >> 6;
        c[1] = (uint8_t)((utf32 & 0x3F) | 0x80);
        utf32 = utf32 >> 6;
        c[0] = (uint8_t)(utf32 | 0xF0);
        c += 4;
    }
    goto step;
utf8:
    res = jsonlite_utf8_sequence_length(*p);
    *c++ = *p++;
    switch (res) {
        case 4: *c++ = *p++;
        case 3: *c++ = *p++;
        default: *c++ = *p++;
    }
    goto step;
done:
    *c = 0;

    return c - (uint8_t *)buffer;
}

size_t jsonlite_token_size_of_uft16(jsonlite_token *ts) {
    return (ts->end - ts->start + 1) * sizeof(uint16_t);
}

size_t jsonlite_token_to_uft16(jsonlite_token *ts, void *buffer) {
    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
	uint32_t code;
    uint16_t utf16;
    uint16_t *c = (uint16_t *)buffer;
    int res;
step:
    if (p == l)         goto done;
    if (*p == '\\')     goto escaped;
    if (*p >= 0x80)     goto utf8;
    *c++ = *p++;
    goto step;
escaped:
    switch (*++p) {
        case 34:    *c++ = '"';     p++; goto step;
        case 47:    *c++ = '/';     p++; goto step;
        case 92:    *c++ = '\\';    p++; goto step;
        case 98:    *c++ = '\b';    p++; goto step;
        case 102:   *c++ = '\f';    p++; goto step;
        case 110:   *c++ = '\n';    p++; goto step;
        case 114:   *c++ = '\r';    p++; goto step;
        case 116:   *c++ = '\t';    p++; goto step;
        default: break;
	}

    /* UTF-16 */
    p++;
    utf16 = jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    *c++ = utf16;
    if (0xD800 > utf16 || utf16 > 0xDBFF) goto step;

    /* UTF-16 Surrogate */
    p += 2;
    utf16 = jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    *c++ = utf16;
    goto step;
utf8:
    res = jsonlite_utf8_sequence_length(*p);
    code = (uint32_t)(*p & (0xFF >> (res + 1)));
    switch (res) {
        case 4: code = (code << 6) | (*++p & 0x3F);
        case 3: code = (code << 6) | (*++p & 0x3F);
        default: code = (code << 6) | (*++p & 0x3F), ++p;
    }

    c += unicode_char_to_utf16(code, c);
    goto step;
done:
    *c = 0;
    return (c - (uint16_t *)buffer) * sizeof(uint16_t);
}

size_t jsonlite_token_size_of_base64_binary(jsonlite_token *ts) {
    return (((ts->end - ts->start) * 3) / 4 + 3) & ~3;
}

size_t jsonlite_token_base64_to_binary(jsonlite_token *ts, void *buffer) {
    size_t length = 0;
    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
    uint8_t *c = buffer;
    uint32_t bytes, i;
next:
    bytes = 0;
    i = 0;
    do {
        if (p == l) goto error;
        i++;
        bytes <<= 6;
        if (0x41 <= *p && *p <= 0x5A) { bytes |= *p++ - 0x41; continue; }
        if (0x61 <= *p && *p <= 0x7A) { bytes |= *p++ - 0x47; continue; }
        if (0x30 <= *p && *p <= 0x39) { bytes |= *p++ + 0x04; continue; }
        if (*p == 0x2B) { bytes |= 0x3E; p++; continue; }
        if (*p == 0x2F) { bytes |= 0x3F; p++; continue; }
        if (*p == '=' && length > 0) {
            switch (l - p) {
                case 1:
                    *c++ = (uint8_t)((bytes >> 16) & 0x000000FF);
                    *c = (uint8_t)((bytes >> 8) & 0x000000FF);
                    return length + 2;
                case 2:
                    *c = (uint8_t)((bytes >> 10) & 0x000000FF);
                    return length + 1;
                default: break;
            }
        }
        if (*p == 0x5C && *++p == 0x2F) { bytes |= 0x3F; p++; continue; }
        goto error;
    } while (i < 4);

    *c++ = (uint8_t)((bytes >> 16)  & 0x000000FF);
    *c++ = (uint8_t)((bytes >> 8)   & 0x000000FF);
    *c++ = (uint8_t)((bytes)        & 0x000000FF);
    length += 3;

    if (p == l) goto done;
    goto next;
error:
    length = 0;
done:
    return length;
}

long jsonlite_token_to_long(jsonlite_token *token) {
    long res = 0;
    int negative = (token->type & jsonlite_number_negative) == jsonlite_number_negative;
    const uint8_t *c = token->start + negative;
    const uint8_t *e = token->end;
    while (c != e) {
        res = res * 10 + (*c++ - '0');
    }

    return negative ? -res : res;
}

long long jsonlite_token_to_long_long(jsonlite_token *token) {
    long long res = 0;
    int negative = (token->type & jsonlite_number_negative) == jsonlite_number_negative;
    const uint8_t *c = token->start + negative;
    const uint8_t *e = token->end;
    while (c != e) {
        res = res * 10 + (*c++ - '0');
    }

    return negative ? -res : res;
}
