#define JSONLITE_AMALGAMATED
#include "jsonlite.h"
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
#include "jsonlite_parser.h"
#endif

#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER

#include <intrin.h>

static uint32_t __inline jsonlite_clz(uint32_t x) {
   unsigned long r = 0;
   _BitScanForward(&r, x);
   return r;
}

#else

#define jsonlite_clz(x) __builtin_clz((x))

#endif

#define CALL_VALUE_CALLBACK(cbs, type, token)   (cbs.type(&cbs.context, token))
#define CALL_STATE_CALLBACK(cbs, type)          (cbs.type(&cbs.context))

#define CHECK_HEX(c)                            \
if ((c) < 48)               goto error_escape;  \
if ((c) > 102)              goto error_escape;  \
if (57 < (c) && (c) < 65)   goto error_escape;  \
if (70 < (c) && (c) < 97)   goto error_escape

#define CASE_NUMBER_TOKEN_END                   \
case 9:                 goto found_token;       \
case 10:                goto found_token;       \
case 13:                goto found_token;       \
case 32:                goto found_token;       \
case 44:                goto found_token;       \
case 93:                goto found_token;       \
case 125:               goto found_token

enum {
    state_start,
    state_object_key,
    state_object_key_end,
    state_colon,
    state_object_comma_end,
    state_array_value_end,
    state_array_comma_end,
    state_key,
    state_value,
    state_end,
    
    state_suspend = 1 << 7
};

typedef uint8_t parse_state;
struct jsonlite_parser_struct {
    const uint8_t *cursor;
    const uint8_t *limit;
    const uint8_t *token_start;
    const uint8_t *buffer;
    
    uint8_t *buffer_own;
    uint8_t *rest;
    size_t rest_size;
    
    parse_state *current;
    const parse_state *last;
    
    jsonlite_result result;
    jsonlite_parser_callbacks callbacks;
} jsonlite_parser_struct;

static void jsonlite_do_parse(jsonlite_parser parser);
static void jsonlite_finish_parse(jsonlite_parser parser);
static void empty_value_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {}
static void empty_state_callback(jsonlite_callback_context *ctx) {}

const jsonlite_parser_callbacks jsonlite_default_callbacks = {
    &empty_state_callback,
    &empty_state_callback,
    &empty_state_callback,
    &empty_state_callback,
    &empty_state_callback,
    &empty_state_callback,
    &empty_state_callback,
    &empty_state_callback,
    &empty_value_callback,
    &empty_value_callback,
    &empty_value_callback,
    {NULL, NULL}
};

size_t jsonlite_parser_estimate_size(size_t depth) {
    depth = depth < 2 ? 32 : depth;
    return sizeof(jsonlite_parser_struct) + depth * sizeof(parse_state);
}

jsonlite_parser jsonlite_parser_init(size_t depth) {
    depth = depth < 2 ? 32 : depth;
    
    jsonlite_parser parser = (jsonlite_parser)calloc(1, jsonlite_parser_estimate_size(depth));
    parser->result = jsonlite_result_unknown;
    parser->callbacks = jsonlite_default_callbacks;
    parser->current = (parse_state *)((uint8_t *)parser + sizeof(jsonlite_parser_struct));
    parser->current[0] = state_end;
    parser->current[1] = state_start;
    parser->last = parser->current + depth;
    parser->current++;
    return parser;
}

jsonlite_result jsonlite_parser_set_callback(jsonlite_parser parser, const jsonlite_parser_callbacks *cbs) {
    if (parser == NULL || cbs == NULL) {
        return jsonlite_result_invalid_argument;
    }

    parser->callbacks = *cbs;
    parser->callbacks.context.parser = parser;
    return jsonlite_result_ok;
}

jsonlite_result jsonlite_parser_get_result(jsonlite_parser parser) {
    if (parser == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    return parser->result;
}

jsonlite_result jsonlite_parser_tokenize(jsonlite_parser parser, const void *buffer, size_t size) {
    if (parser == NULL || buffer == NULL || size == 0) {
        return jsonlite_result_invalid_argument;
    }
    
    if (parser->rest != NULL) {
        size_t total_size = size + parser->rest_size;
        uint8_t *b = (uint8_t *)malloc(total_size);
        memcpy(b, parser->rest, parser->rest_size);  // LCOV_EXCL_LINE
        memcpy(b + parser->rest_size, buffer, size); // LCOV_EXCL_LINE
        
        free(parser->buffer_own);
        free(parser->rest);
        
        parser->buffer = b;
        parser->buffer_own = b;
        parser->cursor = parser->buffer;
        parser->limit = parser->buffer + total_size;
        parser->rest = NULL;
        parser->rest_size = 0;        
    } else {
        parser->buffer = buffer;
        parser->cursor = parser->buffer;
        parser->limit = parser->buffer + size;
    }
    
    jsonlite_do_parse(parser);
    jsonlite_finish_parse(parser);
    return parser->result;
}

jsonlite_result jsonlite_parser_resume(jsonlite_parser parser) {
    if (parser == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    if (parser->result != jsonlite_result_suspended) {
        return jsonlite_result_not_allowed;
    }
    
    jsonlite_do_parse(parser);
    jsonlite_finish_parse(parser);
    return parser->result;
}

jsonlite_result jsonlite_parser_suspend(jsonlite_parser parser) {
    if (parser == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    if (parser->result != jsonlite_result_unknown) {
        return jsonlite_result_not_allowed;
    }
    
    parser->result = jsonlite_result_suspended;
    return jsonlite_result_ok;
}

void jsonlite_parser_release(jsonlite_parser parser) {
    if (parser == NULL) {
        return;
    }
    
    free(parser->buffer_own);
    free(parser->rest);
    free(parser);
}

static void jsonlite_do_parse(jsonlite_parser parser) {
    const uint8_t *c = parser->cursor;
    const uint8_t *l = parser->limit;
    const uint8_t *token_start = NULL;
    const parse_state *last = parser->last;
    parse_state *state = parser->current;
    jsonlite_token token;
    parser->result = jsonlite_result_unknown;
    goto state_selection;
    
structure_finished:
    if (*--state == state_end) goto success;
skip_char_and_spaces:
    c++;
state_selection:
    if (c == l)     goto end_of_stream_space;
    if (*c == ' ')  goto skip_char_and_spaces;
    if (*c == '\n') goto skip_char_and_spaces;
    if (*c == '\r') goto skip_char_and_spaces;
    if (*c == '\t') goto skip_char_and_spaces;
    token_start = c;
    
    if (parser->result != jsonlite_result_unknown) goto end;
    switch (*state) {
        case state_value:               goto value_type_detection;
        case state_key:                 goto string_token_parsing;
        case state_object_key:          goto key_checking;
        case state_object_key_end:      goto key_end_checking;
        case state_colon:               goto colon_checking;
        case state_object_comma_end:    goto object_comma_end_checking;
        case state_array_value_end:     goto array_value_end_checking;
        case state_array_comma_end:     goto array_comma_end_checking;
        case state_start:
            if (*c == '{') goto object_state_machine;
            if (*c == '[') goto array_state_machine;
            goto error_exp_ooa;
        default:
            goto suspend;
    }
    
object_state_machine:
    *state = state_object_key_end;
    CALL_STATE_CALLBACK(parser->callbacks, object_start);
    goto skip_char_and_spaces;
    
array_state_machine:
    *state = state_array_value_end;
    CALL_STATE_CALLBACK(parser->callbacks, array_start);
    goto skip_char_and_spaces;
    
key_checking:
    if (*c != '"') goto error_exp_key;
    *state = state_colon;
    *++state = state_key;
    goto string_token_parsing;
    
colon_checking:
    if (*c != ':') goto error_exp_colon;
    *state = state_object_comma_end;
    *++state = state_value;
    goto skip_char_and_spaces;
    
key_end_checking:
    switch (*c) {
        case '"':
            *state = state_colon;
            if (++state == last) goto error_depth;
            *state = state_key;
            goto string_token_parsing;
        case '}':
            CALL_STATE_CALLBACK(parser->callbacks, object_end);
            goto structure_finished;
    }
    goto error_exp_koe;
    
object_comma_end_checking:
    switch (*c) {
        case ',':
            *state = state_object_key;
            goto skip_char_and_spaces;
        case '}':
            CALL_STATE_CALLBACK(parser->callbacks, object_end);
            goto structure_finished;
    }
    goto error_exp_coe;

array_value_end_checking:
    switch (*c) {
        case ']':
            CALL_STATE_CALLBACK(parser->callbacks, array_end);
            goto structure_finished;
        default:
            *state = state_array_comma_end;
            if (++state == last) goto error_depth;
            *state = state_value;
            goto value_type_detection;
    }
    
array_comma_end_checking:
    switch (*c) {
        case ',':
            *++state = state_value;
            goto skip_char_and_spaces;
        case ']':
            CALL_STATE_CALLBACK(parser->callbacks, array_end);
            goto structure_finished;
    }
    goto error_exp_coe;
    
value_type_detection:
    if (*c == '-' || ('0' <= *c && *c <= '9'))  goto number_parsing;
    if (*c == '"')                              goto string_token_parsing;
    if (*c == 't')                              goto true_token_parsing;
    if (*c == 'f')                              goto false_token_paring;
    if (*c == 'n')                              goto null_token_parsing;
    if (*c == '{')                              goto object_state_machine;
    if (*c == '[')                              goto array_state_machine;
    goto error_exp_value;
    
// State machine for number token parsing.
number_parsing:
    {
        jsonlite_number_type type = jsonlite_number_int;
        token.start = c;        
        switch (*c) {
            case 45: type |= jsonlite_number_negative;      goto test_zero_leading;
            case 48: type |= jsonlite_number_zero_leading;  goto take_exp_frac;
            default: type |= jsonlite_number_digit_leading; goto take_digits;
        }
        
    test_zero_leading:
        if (++c == l)               goto end_of_stream;
        if (49 <= *c && *c <= 57)   goto take_digits;
        if (*c == 48)               goto take_exp_frac;
        goto error_number;
    take_exp_frac:
        if (++c == l)               goto end_of_stream;
        switch (*c) {
            CASE_NUMBER_TOKEN_END;
            case 46:                goto found_fraction;
            case 69:                goto take_exponent;
            case 101:               goto take_exponent;
            default:                goto error_number;
        }
    found_fraction:
        type |= jsonlite_number_frac;
        if (++c == l)               goto end_of_stream;
        if (48 <= *c && *c <= 57)   goto take_frac_number;
        goto error_number;
    take_frac_number:
        if (++c == l)               goto end_of_stream;
        if (48 <= *c && *c <= 57)   goto take_frac_number;
        switch (*c) {
            CASE_NUMBER_TOKEN_END;
            case 69:                goto take_exponent;
            case 101:               goto take_exponent;
            default:                goto error_number;
        }
    take_exponent:
        type |= jsonlite_number_exp;
        if (++c == l)               goto end_of_stream;
        if (48 <= *c && *c <= 57)   goto take_exponent_number;
        switch(*c) {
            case 43:                goto take_exponent_sign;
            case 45:                goto take_exponent_sign;
            default:                goto error_number;
        }
    take_exponent_sign:
        if (++c == l)               goto end_of_stream;
        if (48 <= *c && *c <= 57)   goto take_exponent_number;
        goto error_number;
    take_exponent_number:
        if (++c == l)               goto end_of_stream;
        if (48 <= *c && *c <= 57)   goto take_exponent_number;
        switch(*c) {
            CASE_NUMBER_TOKEN_END;
            default:                goto error_number;
        }
    take_digits:
        if (++c == l)               goto end_of_stream;
        if (48 <= *c && *c <= 57)   goto take_digits;
        switch(*c) {
            CASE_NUMBER_TOKEN_END;
            case 46:                goto found_fraction;
            case 69:                goto take_exponent;
            case 101:               goto take_exponent;
            default:                goto error_number;
        }
    found_token:
        token.end = c;
        token.number_type = type;
        CALL_VALUE_CALLBACK(parser->callbacks, number_found, &token);
        state--;
        goto state_selection;
    }
    
// State machine for string token parsing.
string_token_parsing:
    {
        jsonlite_string_type type = jsonlite_string_ascii;
        uint32_t value, utf32;
        token.start = c + 1;
    next_char:
        if (++c == l)       goto end_of_stream;
        if (*c == '"')      goto string_parsed;
        if (*c == '\\')     goto escaped;
        if (*c >= 0x80)     goto utf8;
        if (*c < 0x20)      goto error_token;
        goto next_char;
    escaped:
        type |= jsonlite_string_escape;
        if (++c == l)       goto end_of_stream;
        switch (*c) {
            case 34:        goto next_char;
            case 47:        goto next_char;
            case 92:        goto next_char;
            case 98:        goto next_char;
            case 102:       goto next_char;
            case 110:       goto next_char;
            case 114:       goto next_char;
            case 116:       goto next_char;
            case 117:       goto hex;
        }
        goto error_escape;
    hex:
        type |= jsonlite_string_unicode_escape;
        if (c++ + 4 >= l)           goto end_of_stream;        
        CHECK_HEX(*c);  value = jsonlite_hex_char_to_uint8(*c++);
        CHECK_HEX(*c);  value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
        CHECK_HEX(*c);  value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
        CHECK_HEX(*c);  value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c);
        
        if ((value & 0xFFFFu) >= 0xFFFEu)           goto error_token;
        if (value >= 0xFDD0u && value <= 0xFDEFu)   goto error_token;
        if (0xD800 > value || value > 0xDBFF)       goto next_char;
        
        // UTF-16 Surrogate
        utf32 = (value - 0xD800) << 10;
        if (c++ + 6 >= l)                           goto end_of_stream;
        if (*c++ != '\\')                           goto error_escape;
        if (*c++ != 'u')                            goto error_escape;        
        CHECK_HEX(*c);  value = jsonlite_hex_char_to_uint8(*c++);
        CHECK_HEX(*c);  value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
        CHECK_HEX(*c);  value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
        CHECK_HEX(*c);  value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c);

        if (value < 0xDC00 || value > 0xDFFF)       goto error_escape;
        utf32 += value - 0xDC00 + 0x10000;
        if ((utf32 & 0x0FFFFu) >= 0x0FFFEu)         goto error_token;
        goto next_char;
    utf8:
        type |= jsonlite_string_utf8;
        int res = jsonlite_clz(((*c) ^ 0xFF) << 0x19);
        utf32 = (*c & (0xFF >> (res + 1)));
        value = 0xAAAAAAAA; // == 1010...
        if (c + res >= l) goto end_of_stream;
        switch (res) {
            case 3: value = (value << 2) | (*++c >> 6); utf32 = (utf32 << 6) | (*c & 0x3F);
            case 2: value = (value << 2) | (*++c >> 6); utf32 = (utf32 << 6) | (*c & 0x3F);
            case 1: value = (value << 2) | (*++c >> 6); utf32 = (utf32 << 6) | (*c & 0x3F);
                if (value != 0xAAAAAAAA)                    goto error_utf8;
                if ((utf32 & 0xFFFFu) >= 0xFFFEu)           goto error_utf8;
                if (utf32 >= 0xFDD0u && utf32 <= 0xFDEFu)   goto error_utf8;
        }
        goto next_char;
    string_parsed:
        token.string_type = type;
        token.end = c;
        parser->cursor = c + 1;
        if (*state == state_value) {
            CALL_VALUE_CALLBACK(parser->callbacks, string_found, &token);
        } else {
            CALL_VALUE_CALLBACK(parser->callbacks, key_found, &token);
        }
        state--;
        goto skip_char_and_spaces;
    }
    
// Primitive tokens states.
true_token_parsing:
    if (c++ + 3 >= l)   goto end_of_stream;
    if (*c++ != 'r')    goto error_token;
    if (*c++ != 'u')    goto error_token;
    if (*c++ != 'e')    goto error_token;
    CALL_STATE_CALLBACK(parser->callbacks, true_found);
    state--;
    goto state_selection;
false_token_paring:
    if (c++ + 4 >= l)   goto end_of_stream;
    if (*c++ != 'a')    goto error_token;
    if (*c++ != 'l')    goto error_token;
    if (*c++ != 's')    goto error_token;
    if (*c++ != 'e')    goto error_token;
    CALL_STATE_CALLBACK(parser->callbacks, false_found);
    state--;
    goto state_selection;
null_token_parsing:
    if (c++ + 3 >= l)   goto end_of_stream;
    if (*c++ != 'u')    goto error_token;
    if (*c++ != 'l')    goto error_token;
    if (*c++ != 'l')    goto error_token;
    CALL_STATE_CALLBACK(parser->callbacks, null_found);
    state--;
    goto state_selection;

// Error handling states.
error_depth:        parser->result = jsonlite_result_depth_limit;               goto end;
error_exp_ooa:      parser->result = jsonlite_result_expected_object_or_array;  goto end;
error_exp_value:    parser->result = jsonlite_result_expected_value;            goto end;
error_exp_koe:      parser->result = jsonlite_result_expected_key_or_end;       goto end;
error_exp_key:      parser->result = jsonlite_result_expected_key;              goto end;
error_exp_colon:    parser->result = jsonlite_result_expected_colon;            goto end;
error_exp_coe:      parser->result = jsonlite_result_expected_comma_or_end;     goto end;
error_escape:       parser->result = jsonlite_result_invalid_escape;            goto end;
error_number:       parser->result = jsonlite_result_invalid_number;            goto end;
error_token:        parser->result = jsonlite_result_invalid_token;             goto end;
error_utf8:         parser->result = jsonlite_result_invalid_utf8;              goto end;
suspend:            parser->result = jsonlite_result_suspended;                 goto end;
// End of stream states.
end_of_stream_space:
    token_start = l;
end_of_stream:
    parser->result = jsonlite_result_end_of_stream;
    goto end;
    
success:
    parser->result = jsonlite_result_ok;
end:
    parser->token_start = token_start;
    parser->current = state;
    parser->cursor = c;
    parser->callbacks.parse_finished(&parser->callbacks.context);
}

static void jsonlite_finish_parse(jsonlite_parser parser) {
    if (parser->result == jsonlite_result_suspended) {
        return;
    }
    
    if (*parser->current == state_end) {
        return;
    }
    
    parser->rest_size = parser->limit - parser->token_start;
    if (parser->rest_size > 0) {
        parser->rest = malloc(parser->rest_size);
        memcpy(parser->rest, parser->token_start, parser->rest_size); // LCOV_EXCL_LINE
    }
}
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
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#define jsonlite_builder_check_depth()                                  \
do {                                                                    \
    if (builder->state - builder->stack >= builder->stack_depth - 1) {  \
        return jsonlite_result_depth_limit;                             \
    }                                                                   \
} while (0)                                             

typedef enum {
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
    
    jsonlite_accept_value = 0
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
    
} jsonlite_accept;

typedef struct jsonlite_write_state {
    int accept;
} jsonlite_write_state;

typedef struct jsonlite_builder_buffer {
    char data[2048];
    char *cursor;
    char *limit;
    struct jsonlite_builder_buffer *next;
} jsonlite_builder_buffer;

typedef struct jsonlite_builder_struct {
    jsonlite_builder_buffer *first;
    jsonlite_builder_buffer *buffer;
    
    jsonlite_write_state *stack;
    jsonlite_write_state *state;
    ptrdiff_t stack_depth;
    
    char *doubleFormat;
    
    size_t indentation;
} jsonlite_builder_struct;

static int jsonlite_builder_accept(jsonlite_builder builder, jsonlite_accept a);
static void jsonlite_builder_pop_state(jsonlite_builder builder);
static void jsonlite_builder_push_buffer(jsonlite_builder builder);
static void jsonlite_builder_prepare_value_writing(jsonlite_builder builder);
static void jsonlite_builder_raw_char(jsonlite_builder builder, char data);
static void jsonlite_builder_write_uft8(jsonlite_builder builder, const char *data, size_t length);
static void jsonlite_builder_raw(jsonlite_builder builder, const void *data, size_t length);
static void jsonlite_builder_repeat(jsonlite_builder builder, const char ch, size_t count);

jsonlite_builder jsonlite_builder_init(size_t depth) {
    jsonlite_builder builder;
    
    depth = depth < 2 ? 2 : depth;
    
    builder = (jsonlite_builder)calloc(1, sizeof(jsonlite_builder_struct) + depth * sizeof(jsonlite_write_state));
    builder->first = (jsonlite_builder_buffer *)malloc(sizeof(jsonlite_builder_buffer));
    builder->buffer = builder->first;
    builder->buffer->cursor = builder->buffer->data;
    builder->buffer->limit = builder->buffer->data + sizeof(builder->buffer->data);
    builder->buffer->next = NULL;
    
    builder->stack = (jsonlite_write_state *)((uint8_t *)builder + sizeof(jsonlite_builder_struct));
    builder->stack_depth = depth;
    builder->state = builder->stack;
    builder->state->accept = jsonlite_accept_object_begin | jsonlite_accept_array_begin;
    
    builder->indentation = 0;
    jsonlite_builder_set_double_format(builder, "%.16g");
    return builder;
}

jsonlite_result jsonlite_builder_release(jsonlite_builder builder) {
	jsonlite_builder_buffer *b = NULL;
    void *prev;
    
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }

    for (b = builder->first; b != NULL;) {
        prev = b;        
        b = b->next;
        free(prev);
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

static int jsonlite_builder_accept(jsonlite_builder builder, jsonlite_accept a) {
    return (builder->state->accept & a) == a;
}

static void jsonlite_builder_push_state(jsonlite_builder builder) {
    builder->state++;
}

static void jsonlite_builder_pop_state(jsonlite_builder builder) {
    jsonlite_write_state *ws = --builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
        ws->accept = jsonlite_accept_continue_array;
    } else {
        ws->accept = jsonlite_accept_continue_object;
    }
}

static void jsonlite_builder_push_buffer(jsonlite_builder builder) {
    jsonlite_builder_buffer *buffer = builder->buffer;
    buffer->next = malloc(sizeof(jsonlite_builder_buffer));
    buffer = builder->buffer = buffer->next;
    
    buffer->cursor = buffer->data;
    buffer->limit = buffer->data + sizeof(buffer->data);
    buffer->next = NULL;
}

static void jsonlite_builder_prepare_value_writing(jsonlite_builder builder) {
    jsonlite_write_state *ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
        if (jsonlite_builder_accept(builder, jsonlite_accept_next) ) {
            jsonlite_builder_raw_char(builder, ',');
        }
        if (builder->indentation != 0) {
            jsonlite_builder_raw_char(builder, '\r');
            jsonlite_builder_repeat(builder, ' ', (builder->state - builder->stack) * builder->indentation);
        }
    } else {
        ws->accept &= ~jsonlite_accept_value;
        ws->accept |= jsonlite_accept_key;
    }
    ws->accept |= jsonlite_accept_next;
}

jsonlite_result jsonlite_builder_object_begin(jsonlite_builder builder) {
    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    jsonlite_builder_check_depth();

    if (jsonlite_builder_accept(builder, jsonlite_accept_object_begin)) {
        jsonlite_builder_prepare_value_writing(builder);
        jsonlite_builder_push_state(builder);
        builder->state->accept = jsonlite_accept_object_end | jsonlite_accept_key;
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
        jsonlite_builder_push_state(builder);
        builder->state->accept = jsonlite_accept_array_end 
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
    size_t i;
    jsonlite_builder_raw_char(builder, '\"');
    for (i = 0; i < length; i++) {
        switch (data[i]) {
            case '"':
                jsonlite_builder_raw(builder, "\\\"", 2);
                break;
            case '\\':
                jsonlite_builder_raw(builder, "\\\\", 2);
                break;
            case '\b':
                jsonlite_builder_raw(builder, "\\b", 2);
                break;
            case '\f':
                jsonlite_builder_raw(builder, "\\f", 2);
                break;
            case '\n':
                jsonlite_builder_raw(builder, "\\n", 2);
                break;
            case '\r':
                jsonlite_builder_raw(builder, "\\r", 2);
                break;
            case '\t':
                jsonlite_builder_raw(builder, "\\t", 2);
                break;
            default:
                jsonlite_builder_raw_char(builder, data[i]);
                break;
        }
    }
    jsonlite_builder_raw_char(builder, '\"');
}

jsonlite_result jsonlite_builder_key(jsonlite_builder builder, const char *data, size_t length) {
	jsonlite_write_state *ws;

    if (builder == NULL || data == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    ws = builder->state;

    if (jsonlite_builder_accept(builder, jsonlite_accept_key) ) {
        if (jsonlite_builder_accept(builder, jsonlite_accept_next) ) {
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
        ws->accept = jsonlite_accept_value;
        return jsonlite_result_ok;
    }
    
    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_string(jsonlite_builder builder, const char *data, size_t length) {
	jsonlite_write_state *ws;

    if (builder == NULL || data == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    ws = builder->state;

    if (jsonlite_builder_accept(builder, jsonlite_accept_value) ) {
        jsonlite_builder_prepare_value_writing(builder);
        jsonlite_builder_write_uft8(builder, data, length);
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
            ws->accept = jsonlite_accept_continue_array;
        } else {
            ws->accept = jsonlite_accept_continue_object;
        }
        return jsonlite_result_ok;
    }
    
    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_int(jsonlite_builder builder, long long value) {
	jsonlite_write_state *ws;
	char buff[128];
	int size = 0;

    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    ws = builder->state;

    if (jsonlite_builder_accept(builder, jsonlite_accept_value) ) {
        jsonlite_builder_prepare_value_writing(builder);
        size = sprintf(buff, "%lld", value);
        jsonlite_builder_raw(builder, buff, size);
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
            ws->accept = jsonlite_accept_continue_array;
        } else {
            ws->accept = jsonlite_accept_continue_object;
        }
        return jsonlite_result_ok;
    }
    
    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_double(jsonlite_builder builder, double value) {
	jsonlite_write_state *ws;
	char buff[128];
	int size = 0;

    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    ws = builder->state;

    if (jsonlite_builder_accept(builder, jsonlite_accept_value) ) {
        jsonlite_builder_prepare_value_writing(builder);
        size = sprintf(buff, builder->doubleFormat, value);
        jsonlite_builder_raw(builder, buff, size);
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
            ws->accept = jsonlite_accept_continue_array;
        } else {
            ws->accept = jsonlite_accept_continue_object;
        }
        return jsonlite_result_ok;
    }
    
    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_true(jsonlite_builder builder) {
	static const char value[] = "true";
    jsonlite_write_state *ws;

    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
	ws = builder->state;
    if (!(jsonlite_builder_accept(builder, jsonlite_accept_value) )) {
        
        return jsonlite_result_not_allowed;
    }
    
    jsonlite_builder_prepare_value_writing(builder);
    jsonlite_builder_raw(builder, (char *)value, sizeof(value) - 1);
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
        ws->accept = jsonlite_accept_continue_array;
    } else {
        ws->accept = jsonlite_accept_continue_object;
    }
    return jsonlite_result_ok;
}

jsonlite_result jsonlite_builder_false(jsonlite_builder builder) {
	static const char value[] = "false";
	jsonlite_write_state *ws;

    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    ws = builder->state;
    if (!(jsonlite_builder_accept(builder, jsonlite_accept_value) )) {
        
        return jsonlite_result_not_allowed;
    }

    jsonlite_builder_prepare_value_writing(builder);
    jsonlite_builder_raw(builder, (char *)value, sizeof(value) - 1);
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
        ws->accept = jsonlite_accept_continue_array;
    } else {
        ws->accept = jsonlite_accept_continue_object;
    }
    return jsonlite_result_ok;
}

jsonlite_result jsonlite_builder_null(jsonlite_builder builder) {
	static const char value[] = "null";
    jsonlite_write_state *ws;

    if (builder == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
	ws = builder->state;
    if (!(jsonlite_builder_accept(builder, jsonlite_accept_value) )) {
        
        return jsonlite_result_not_allowed;
    }
    
    jsonlite_builder_prepare_value_writing(builder);
    jsonlite_builder_raw(builder, (char *)value, sizeof(value) - 1);
    if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
        ws->accept = jsonlite_accept_continue_array;
    } else {
        ws->accept = jsonlite_accept_continue_object;
    }
    return jsonlite_result_ok;
}
 
static void jsonlite_builder_raw(jsonlite_builder builder, const void *data, size_t length) {
    jsonlite_builder_buffer *buffer = builder->buffer;
    size_t write_limit = buffer->limit - buffer->cursor;
    if (write_limit >= length) {
        memcpy(buffer->cursor, data, length); // LCOV_EXCL_LINE
        buffer->cursor += length;
    } else {
        memcpy(buffer->cursor, data, write_limit); // LCOV_EXCL_LINE
        buffer->cursor += write_limit;
        
        jsonlite_builder_push_buffer(builder);
        jsonlite_builder_raw(builder, (char *)data + write_limit, length - write_limit);
    }
}

static void jsonlite_builder_repeat(jsonlite_builder builder, const char ch, size_t count) {
    jsonlite_builder_buffer *buffer = builder->buffer;
    size_t write_limit = buffer->limit - buffer->cursor;
    if (write_limit >= count) {
        memset(buffer->cursor, ch, count); // LCOV_EXCL_LINE
        buffer->cursor += count;
    } else {
        memset(buffer->cursor, ch, write_limit); // LCOV_EXCL_LINE
        buffer->cursor += write_limit;
        
        jsonlite_builder_push_buffer(builder);
        jsonlite_builder_repeat(builder, ch, count - write_limit);
    }
}

static  void jsonlite_builder_raw_char(jsonlite_builder builder, char data) {
    jsonlite_builder_buffer *buffer = builder->buffer;
    if (buffer->cursor >= buffer->limit) {
        jsonlite_builder_push_buffer(builder);
    }
    *builder->buffer->cursor++ = data;
}

jsonlite_result jsonlite_builder_raw_key(jsonlite_builder builder, const void *data, size_t length) {
	jsonlite_write_state *ws;

    if (builder == NULL || data == NULL || length == 0) {
        return jsonlite_result_invalid_argument;
    }
    
    ws = builder->state;
    if (jsonlite_builder_accept(builder, jsonlite_accept_key) ) {
        if (jsonlite_builder_accept(builder, jsonlite_accept_next) ) {
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
        ws->accept = jsonlite_accept_value;
        return jsonlite_result_ok;
    }
    
    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_raw_string(jsonlite_builder builder, const void *data, size_t length) {
    jsonlite_write_state *ws;
    
    if (builder == NULL || data == NULL || length == 0) {
        return jsonlite_result_invalid_argument;
    }
    
    ws = builder->state;
    
    if (jsonlite_builder_accept(builder, jsonlite_accept_value) ) {
        jsonlite_builder_prepare_value_writing(builder);
        jsonlite_builder_raw_char(builder, '\"');
        jsonlite_builder_raw(builder, data, length);
        jsonlite_builder_raw_char(builder, '\"');
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
            ws->accept = jsonlite_accept_continue_array;
        } else {
            ws->accept = jsonlite_accept_continue_object;
        }
        return jsonlite_result_ok;
    }
    
    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_raw_value(jsonlite_builder builder, const void *data, size_t length) {
	jsonlite_write_state *ws;

    if (builder == NULL || data == NULL || length == 0) {
        return jsonlite_result_invalid_argument;
    }
    
    ws = builder->state;

    if (jsonlite_builder_accept(builder, jsonlite_accept_value) ) {
        jsonlite_builder_prepare_value_writing(builder);
        jsonlite_builder_raw(builder, data, length);
        if (jsonlite_builder_accept(builder, jsonlite_accept_values_only) ) {
            ws->accept = jsonlite_accept_continue_array;
        } else {
            ws->accept = jsonlite_accept_continue_object;
        }
        return jsonlite_result_ok;
    }
    
    return jsonlite_result_not_allowed;
}

jsonlite_result jsonlite_builder_data(jsonlite_builder builder, char **data, size_t *size) {
	jsonlite_builder_buffer *b;
	char *buff = NULL;

    if (builder == NULL || data == NULL || size == NULL) {
        return jsonlite_result_invalid_argument;
    }
 
	*size = 0;
    for (b = builder->first; b != NULL; b = b->next) {
        *size +=  b->cursor - b->data;
    }
    
    if (*size == 0) {
        return jsonlite_result_not_allowed;
    }
    
    *data = (char*)calloc(*size, 1);
    buff = *data; 
    for (b = builder->first; b != NULL; b = b->next) {
        size_t s = b->cursor - b->data;
        memcpy(buff, b->data, s); // LCOV_EXCL_LINE
        buff += s;
    }
    return jsonlite_result_ok;
}
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
#include "../include/jsonlite_token.h"
#endif

#include <stdlib.h>

#ifdef _MSC_VER
#include <intrin.h>

static uint32_t __inline jsonlite_clz( uint32_t x ) {
   unsigned long r = 0;
   _BitScanForward(&r, x);
   return r;
}

#else

#define jsonlite_clz(x) __builtin_clz((x))

#endif

uint8_t jsonlite_hex_char_to_uint8(uint8_t c) {
    uint8_t res = 0xFF;
    if (c >= '0' && c <= '9') {
        res = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        res = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        res = c - 'A' + 10;
    }
    return res;
}

static int unicode_char_to_utf16(uint32_t ch, uint16_t *utf16) {
    uint32_t v = ch - 0x10000;
    uint32_t vh = v >> 10;
    uint32_t vl = v & 0x3FF;
	if (ch <= 0xFFFF) {
        *utf16 = (uint16_t)ch;
        return 1;
    }
    
    *utf16++ = (uint16_t)(0xD800 + vh);
    *utf16 = (uint16_t)(0xDC00 + vl);
    return 2;
}

size_t jsonlite_token_decode_size_for_uft8(jsonlite_token *ts) {
    if (ts == NULL) {
        return 0;
    }
    
    return ts->end - ts->start + 1;
}

size_t jsonlite_token_decode_to_uft8(jsonlite_token *ts, uint8_t **buffer) {
    size_t size = jsonlite_token_decode_size_for_uft8(ts);
    if (size == 0 || buffer == NULL) {
        return 0;
    }
    
    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
    uint32_t value, utf32;
  	uint8_t *c = *buffer = (uint8_t *)malloc(size);
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
	}

    // UTF-16    
    p++;
    utf32 = jsonlite_hex_char_to_uint8(*p++);
    utf32 = (uint32_t)(utf32 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf32 = (uint32_t)(utf32 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf32 = (uint32_t)(utf32 << 4) | jsonlite_hex_char_to_uint8(*p++);
    if (0xD800 > utf32 || utf32 > 0xDBFF) goto encode;
    
    // UTF-16 Surrogate
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
        c[1] = (uint8_t)(utf32 & 0x3F) | 0x80;
        utf32 = utf32 >> 6;
        c[0] = (uint8_t)utf32 | 0xC0;
        c += 2;
    } else if (utf32 < 0x10000) {
        c[2] = (uint8_t)(utf32 & 0x3F) | 0x80;
        utf32 = utf32 >> 6;
        c[1] = (uint8_t)(utf32 & 0x3F) | 0x80;
        utf32 = utf32 >> 6;
        c[0] = (uint8_t)utf32 | 0xE0;
        c += 3;
    } else {
        c[3] = (uint8_t)(utf32 & 0x3F) | 0x80;
        utf32 = utf32 >> 6;
        c[2] = (uint8_t)(utf32 & 0x3F) | 0x80;
        utf32 = utf32 >> 6;
        c[1] = (uint8_t)(utf32 & 0x3F) | 0x80;
        utf32 = utf32 >> 6;
        c[0] = (uint8_t)utf32 | 0xF0;
        c += 4;
    }
    goto step;
utf8:
    res = jsonlite_clz(((*p) ^ 0xFF) << 0x19);
    *c++ = *p++;
    switch (res) {
        case 3: *c++ = *p++;
        case 2: *c++ = *p++;
        case 1: *c++ = *p++;
    }
    goto step;
done:
    *c = 0;
    return c - *buffer;
}

size_t jsonlite_token_decode_size_for_uft16(jsonlite_token *ts) {
    if (ts == NULL) {
        return 0;
    }
    
    return (ts->end - ts->start + 1) * sizeof(uint16_t);
}

size_t jsonlite_token_decode_to_uft16(jsonlite_token *ts, uint16_t **buffer) {
    size_t size = jsonlite_token_decode_size_for_uft16(ts);
    if (size == 0 || buffer == NULL) {
        return 0;
    }
    
    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
    uint16_t utf16;
    uint16_t *c = *buffer = (uint16_t *)malloc(size);
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
	}
    
    // UTF-16
    p++;
    utf16 = jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    *c++ = utf16;
    if (0xD800 > utf16 || utf16 > 0xDBFF) goto step;
    
    // UTF-16 Surrogate
    p += 2;
    utf16 = jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    utf16 = (uint16_t)(utf16 << 4) | jsonlite_hex_char_to_uint8(*p++);
    *c++ = utf16;
    goto step;
utf8:
    res = jsonlite_clz(((*p) ^ 0xFF) << 0x19);
    uint32_t code = (*p & (0xFF >> (res + 1)));
    switch (res) {
        case 3: code = (code << 6) | (*++p & 0x3F);
        case 2: code = (code << 6) | (*++p & 0x3F);
        case 1: code = (code << 6) | (*++p & 0x3F);
        case 0: ++p;
    }
    
    c += unicode_char_to_utf16(code, c);
    goto step;
done:
    *c = 0;
    return (c - *buffer) * sizeof(uint16_t);
}
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
static uint32_t jsonlite_hash(const uint8_t *data, size_t len);

jsonlite_token_pool jsonlite_token_pool_create(jsonlite_token_pool_release_value_fn release_fn) {
    jsonlite_token_pool pool = (jsonlite_token_pool)calloc(1, sizeof(jsonlite_token_pool_struct));
    pool->release_fn = release_fn;
    return pool;
}

void jsonlite_token_pool_copy_tokens(jsonlite_token_pool pool) {
    jsonlite_token_bucket *b;
    size_t size = pool->content_pool_size;
    for (int i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        b = pool->buckets[i];
        if (jsonlite_bucket_not_copied(pool, b)) {
            size += b->end - b->start;
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
    for (int i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        b = pool->buckets[i];
        if (b == NULL) {
            continue;
        }
        
        if (jsonlite_bucket_not_copied(pool, b)) {
            size_t length = b->end - b->start;
            memcpy(p, b->start, length); // LCOV_EXCL_LINE
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
    if (pool == NULL) {
        return;
    }

    for (int i = 0; i < JSONLITE_TOKEN_POOL_FRONT; i++) {
        jsonlite_token_bucket *bucket = pool->buckets[i];
        if (bucket == NULL) {
            continue;
        }
        
        if (pool->release_fn != NULL) {
            size_t count = pool->buckets_length[i];
            for (int j = 0; j < count; j++, bucket++) {
                pool->release_fn((void *)bucket->value);           
            }
        }

        free(pool->buckets[i]);
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
    jsonlite_token_bucket *bucket = pool->buckets[index];
    size_t count = pool->buckets_length[index];
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
    return memcmp(t1, t2, length) == 0 ? 1 : 0;
}

static void jsonlite_extend_capacity(jsonlite_token_pool pool, int index) {
    size_t capacity = pool->buckets_capacity[index];
    if (capacity == 0) {
        capacity = 0x10;
    }
    
    size_t size = capacity * sizeof(jsonlite_token_bucket);
    jsonlite_token_bucket *b = pool->buckets[index];
	jsonlite_token_bucket *extended = (jsonlite_token_bucket *)malloc(2 * size);
    
    if (b != NULL) {
        memcpy(extended, b, size); // LCOV_EXCL_LINE
        free(b);
    }
    
    pool->buckets[index] = extended;
    pool->buckets_capacity[index] = 2 * capacity;
}

static int jsonlite_bucket_not_copied(jsonlite_token_pool pool, jsonlite_token_bucket *b) {
    if (b == NULL) {
        return 0;
    }
    
    int res = b->start < pool->content_pool;
    res |= b->start >= pool->content_pool + pool->content_pool_size;
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

static uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed )
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
    
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    
    // Initialize the hash to a 'random' value
    
    uint32_t h = seed ^ len;
    
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
    return MurmurHash2(data, (int)len, 0);
}
