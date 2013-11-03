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
#include "../include/jsonlite_builder.h"
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define jsonlite_builder_check_depth()          \
do {                                            \
    if (builder->state >= builder->limit) {     \
        return jsonlite_result_depth_limit;     \
    }                                           \
} while (0)

enum {
    jsonlite_accept_object_begin    = 0x0001,
    jsonlite_accept_object_end      = 0x0002,
    jsonlite_accept_array_begin     = 0x0004,
    jsonlite_accept_array_end       = 0x0008,
    jsonlite_accept_key             = 0x0010,
    jsonlite_accept_string          = 0x0020,
    jsonlite_accept_number          = 0x0040,
    jsonlite_accept_boolean         = 0x0080,
    jsonlite_accept_null            = 0x0100,
    jsonlite_accept_values_only     = 0x0200,
    jsonlite_accept_next            = 0x0400,

    jsonlite_accept_value           = 0
                                    | jsonlite_accept_object_begin
                                    | jsonlite_accept_array_begin
                                    | jsonlite_accept_string
                                    | jsonlite_accept_number
                                    | jsonlite_accept_boolean
                                    | jsonlite_accept_null,
    jsonlite_accept_continue_object = 0
                                    | jsonlite_accept_next
                                    | jsonlite_accept_key
                                    | jsonlite_accept_object_end,
    jsonlite_accept_continue_array = 0
                                    | jsonlite_accept_next
                                    | jsonlite_accept_values_only
                                    | jsonlite_accept_value
                                    | jsonlite_accept_array_end
};

typedef uint16_t jsonlite_write_state;
typedef struct jsonlite_builder_struct {
    jsonlite_write_state *state;
    jsonlite_write_state *limit;
    jsonlite_write_state *stack;
    jsonlite_stream stream;

    size_t indentation;
    char *doubleFormat;
} jsonlite_builder_struct;

static int jsonlite_builder_accept(jsonlite_builder builder, jsonlite_write_state a);
static void jsonlite_builder_pop_state(jsonlite_builder builder);
static void jsonlite_builder_prepare_value_writing(jsonlite_builder builder);
static void jsonlite_builder_raw_char(jsonlite_builder builder, char data);
static void jsonlite_builder_write_uft8(jsonlite_builder builder, const char *data, size_t length);
static void jsonlite_builder_raw(jsonlite_builder builder, const void *data, ptrdiff_t length);
static void jsonlite_builder_repeat(jsonlite_builder builder, const char ch, ptrdiff_t count);

jsonlite_builder jsonlite_builder_init(size_t depth, jsonlite_stream stream) {
    jsonlite_builder builder;

    depth = depth < 2 ? 2 : depth;

    builder = malloc(sizeof(jsonlite_builder_struct) + depth * sizeof(jsonlite_write_state));
    builder->state = (jsonlite_write_state *)((uint8_t *)builder + sizeof(jsonlite_builder_struct));
    builder->limit = builder->state + depth - 1;
    builder->stack = builder->state;
    builder->stream = stream;
    builder->indentation = 0;
    *builder->state = jsonlite_accept_object_begin | jsonlite_accept_array_begin;
    jsonlite_builder_set_double_format(builder, "%.16g");
    return builder;
}

jsonlite_result jsonlite_builder_release(jsonlite_builder builder) {
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

    free(builder->doubleFormat);
    free(builder);
    return jsonlite_result_ok;
}

jsonlite_result jsonlite_builder_set_indentation(jsonlite_builder builder, size_t indentation) {
    if (builder != NULL) {
        builder->indentation = indentation;
        return jsonlite_result_ok;
    }
    return jsonlite_result_invalid_argument;
}

jsonlite_result jsonlite_builder_set_double_format(jsonlite_builder builder, const char *format) {
    if (builder != NULL && format != NULL) {
        builder->doubleFormat = strdup(format);
        return jsonlite_result_ok;
    }
    return jsonlite_result_invalid_argument;
}

static int jsonlite_builder_accept(jsonlite_builder builder, jsonlite_write_state a) {
    return (*builder->state & a) == a;
}

static void jsonlite_builder_pop_state(jsonlite_builder builder) {
    jsonlite_write_state *ws = --builder->state;
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
            jsonlite_builder_raw_char(builder, '\r');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }
    } else {
        *ws &= ~jsonlite_accept_value;
        *ws |= jsonlite_accept_key;
    }
    *ws |= jsonlite_accept_next;
}

jsonlite_result jsonlite_builder_object_begin(jsonlite_builder builder) {
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

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
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

    if (jsonlite_builder_accept(builder, jsonlite_accept_object_end)) {
        jsonlite_builder_pop_state(builder);
        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\r');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }
        jsonlite_builder_raw_char(builder, '}');
        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_array_begin(jsonlite_builder builder) {
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

    jsonlite_builder_check_depth();

    if (jsonlite_builder_accept(builder, jsonlite_accept_array_begin)) {
        jsonlite_builder_prepare_value_writing(builder);
        *++builder->state = jsonlite_accept_array_end
            | jsonlite_accept_value
            | jsonlite_accept_values_only;
        jsonlite_builder_raw_char(builder, '[');
        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_array_end(jsonlite_builder builder) {
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

    if (jsonlite_builder_accept(builder, jsonlite_accept_array_end)) {
        jsonlite_builder_pop_state(builder);
        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\r');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }
        jsonlite_builder_raw_char(builder, ']');
        return jsonlite_result_ok;
    }

    return jsonlite_result_not_allowed;
}

static void jsonlite_builder_write_uft8(jsonlite_builder builder, const char *data, size_t length) {
    char b[2] = "\\?";
    const char *c = data;
    const char *p = data;
    const char *l = data + length;
    
    jsonlite_builder_raw_char(builder, '\"');
next:
    if (c == l)                     goto end;
    switch (*c) {
        case '"':   b[1] = '"';     goto flush;
        case '\\':  b[1] = '\\';    goto flush;
        case '\b':  b[1] = 'b';     goto flush;
        case '\f':  b[1] = 'f';     goto flush;
        case '\n':  b[1] = 'n';     goto flush;
        case '\r':  b[1] = 'r';     goto flush;
        case '\t':  b[1] = 't';     goto flush;
        default:    c++;            goto next;
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

jsonlite_result jsonlite_builder_key(jsonlite_builder builder, const char *data, size_t length) {
    if (builder == NULL || data == NULL) {
        return jsonlite_result_invalid_argument;
    }

    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_key)) {
        if (jsonlite_builder_accept(builder, jsonlite_accept_next)) {
            jsonlite_builder_raw_char(builder, ',');
        }
        
        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\r');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }
        
        jsonlite_builder_write_uft8(builder, data, length);
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

jsonlite_result jsonlite_builder_string(jsonlite_builder builder, const char *data, size_t length) {
    if (builder == NULL || data == NULL) {
        return jsonlite_result_invalid_argument;
    }

    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_value)) {
        jsonlite_builder_prepare_value_writing(builder);
        jsonlite_builder_write_uft8(builder, data, length);
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
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

    char buff[64];
	int size = 0;
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_value)) {
        jsonlite_builder_prepare_value_writing(builder);
        size = sprintf(buff, "%lld", value);
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
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

    char buff[64];
	int size = 0;
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_value)) {
        jsonlite_builder_prepare_value_writing(builder);
        size = sprintf(buff, builder->doubleFormat, value);
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
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

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
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

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
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

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

static void jsonlite_builder_raw(jsonlite_builder builder, const void *data, ptrdiff_t length) {
    jsonlite_stream_write(builder->stream, data, length);
}

static void jsonlite_builder_repeat(jsonlite_builder builder, const char ch, ptrdiff_t count) {
    ptrdiff_t i = 0;
    for (; i < count; i++) {
        jsonlite_stream_write(builder->stream, &ch, 1);
    }
}

static  void jsonlite_builder_raw_char(jsonlite_builder builder, char data) {
    jsonlite_stream_write(builder->stream, &data, 1);
}

jsonlite_result jsonlite_builder_raw_key(jsonlite_builder builder, const void *data, size_t length) {
    if (builder == NULL || data == NULL || length == 0) {
        return jsonlite_result_invalid_argument;
    }

    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_key)) {
        if (jsonlite_builder_accept(builder, jsonlite_accept_next)) {
            jsonlite_builder_raw(builder, ",", 1);
        }

        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\r');
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
    if (builder == NULL || data == NULL || length == 0) {
        return jsonlite_result_invalid_argument;
    }

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
    if (builder == NULL || data == NULL || length == 0) {
        return jsonlite_result_invalid_argument;
    }

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
