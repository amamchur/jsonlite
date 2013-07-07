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

#include "../include/jsonlite_parser.h"
#include <stdlib.h>
#include <string.h>

#define JSONLITE_FCS        static int

#ifdef _MSC_VER

#include <intrin.h>

#define likely(x)	(x)
#define unlikely(x) (x)

static uint32_t __inline jsonlite_clz( uint32_t x ) {
   unsigned long r = 0;
   _BitScanForward(&r, x);
   return r;
}

#else

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)
#define jsonlite_clz(x) __builtin_clz((x))

#endif

#define CALL_VALUE_CALLBACK(cbs, type, jt)  (cbs.type(&cbs.context, jt))
#define CALL_STATE_CALLBACK(cbs, type)      (cbs.type(&cbs.context))

#define CHECK_LIMIT(c, l)                                   \
do {                                                        \
    if ((c) >= (l)) {                                       \
        parser->result = jsonlite_result_end_of_stream;     \
        return 0;                                           \
    }                                                       \
} while (0)

#define CHECK_HEX(c)                            \
if ((c) < 48)               goto error_escape;  \
if ((c) > 102)              goto error_escape;  \
if (57 < (c) && (c) < 65)   goto error_escape;  \
if (70 < (c) && (c) < 97)   goto error_escape

#define CHECK_LIMIT_RET_EOS(c, l) if ((c) >= (l)) return jsonlite_result_end_of_stream

struct jsonlite_parse_state_struct;

typedef struct jsonlite_parse_state_struct* jsonlite_parse_state;
typedef int (*jsonlite_parse_function)(jsonlite_parser, jsonlite_parse_state);

typedef struct jsonlite_parse_state_struct {
    jsonlite_parse_function function;
} jsonlite_parse_state_struct;

struct jsonlite_parser_struct {
    jsonlite_result result;
    
    const uint8_t *cursor;
    const uint8_t *limit;
    const uint8_t *error_pos;
    const uint8_t *token_start;
    const uint8_t *buffer;
    
    uint8_t *buffer_own;
    uint8_t *tail;
    size_t tail_size;
    size_t depth;
    
    jsonlite_parse_state top;
    jsonlite_parse_state stack;
    jsonlite_parser_callbacks callbacks;
} jsonlite_parser_struct;

static void jsonlite_do_parse(jsonlite_parser parser);
static void jsonlite_finish_parse(jsonlite_parser parser);

JSONLITE_FCS set_error(jsonlite_parser parser, const uint8_t *pos, jsonlite_result result);

JSONLITE_FCS take_object_or_array(jsonlite_parser parser, jsonlite_parse_state jt);
JSONLITE_FCS take_value(jsonlite_parser parser, jsonlite_parse_state jt);
JSONLITE_FCS take_object_key_end(jsonlite_parser parser, jsonlite_parse_state jt);
JSONLITE_FCS take_object_key(jsonlite_parser parser, jsonlite_parse_state jt);
JSONLITE_FCS take_object_colon(jsonlite_parser parser, jsonlite_parse_state jt);
JSONLITE_FCS take_object_comma_end(jsonlite_parser parser, jsonlite_parse_state jt);
JSONLITE_FCS take_array_value_end(jsonlite_parser parser, jsonlite_parse_state jt);
JSONLITE_FCS take_array_comma_end(jsonlite_parser parser, jsonlite_parse_state jt);
JSONLITE_FCS take_key(jsonlite_parser parser, jsonlite_parse_state jt);

JSONLITE_FCS take_string(jsonlite_parser parser);
JSONLITE_FCS take_number(jsonlite_parser parser);
JSONLITE_FCS take_true(jsonlite_parser parser);
JSONLITE_FCS take_false(jsonlite_parser parser);
JSONLITE_FCS take_null(jsonlite_parser parser);

static int skip_whitespace(jsonlite_parser parser);
static void empty_value_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
}

static void empty_state_callback(jsonlite_callback_context *ctx) {
}

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
    depth = depth == 0 ? 32 : depth;
    return sizeof(jsonlite_parser_struct) + depth * sizeof(jsonlite_parse_state_struct);
}

jsonlite_parser jsonlite_parser_init(size_t depth) {
    jsonlite_parser parser;
    depth = depth == 0 ? 32 : depth;
    
    parser = (jsonlite_parser)malloc(jsonlite_parser_estimate_size(depth));
    parser->cursor = NULL;
    parser->limit = NULL;
    parser->token_start = NULL;
    parser->buffer = NULL;
    parser->buffer_own = NULL;
    parser->tail = NULL;
    parser->tail_size = 0;
    parser->result = jsonlite_result_unknown;
    parser->top = NULL;
    parser->stack = (jsonlite_parse_state)((uint8_t *)parser + sizeof(jsonlite_parser_struct));
    parser->depth = depth;
    parser->callbacks = jsonlite_default_callbacks;
    parser->error_pos = NULL;
    return parser;
}

jsonlite_result jsonlite_parser_set_callback(jsonlite_parser parser, const jsonlite_parser_callbacks *cbs) {
    if (parser == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    if (cbs == NULL) {
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
    if (parser == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    if (buffer == NULL) {
        return jsonlite_result_invalid_argument;
    }
    
    if (size == 0) {
        return jsonlite_result_invalid_argument;
    }
    
    if (parser->tail != NULL) {
        size_t total_size = size + parser->tail_size;
        uint8_t *b = (uint8_t *)malloc(total_size);
        memcpy(b, parser->tail, parser->tail_size);  // LCOV_EXCL_LINE
        memcpy(b + parser->tail_size, buffer, size); // LCOV_EXCL_LINE
        
        free(parser->buffer_own);
        free(parser->tail);
        
        parser->buffer = b;
        parser->buffer_own = b;
        parser->cursor = parser->buffer;
        parser->limit = parser->buffer + total_size;
        parser->tail = NULL;
        parser->tail_size = 0;        
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
    free(parser->tail);
    free(parser);
}

static void jsonlite_do_parse(jsonlite_parser parser) {
    jsonlite_parse_state stack = parser->stack, top = parser->top, limit = stack + parser->depth - 1;    
    parser->result = jsonlite_result_unknown;
    
    if (likely(top == NULL)) {
        if (likely(skip_whitespace(parser))) {
            top = stack + take_object_or_array(parser, stack);
        } else {
            parser->result = jsonlite_result_end_of_stream;
            parser->top = top;
            parser->callbacks.parse_finished(&parser->callbacks.context);
            return;
        }
    }
    
    while (top >= stack && top < limit) {
        if (unlikely(parser->result != jsonlite_result_unknown)) {
            parser->top = top;
            parser->callbacks.parse_finished(&parser->callbacks.context);
            return;
        }
 
        if (likely(skip_whitespace(parser))) {
            top += top->function(parser, top);
        } else {
            parser->result = jsonlite_result_end_of_stream;
        }
    }
        
    parser->result = top >= limit ? jsonlite_result_depth_limit : jsonlite_result_ok;
    parser->top = top;
    parser->callbacks.parse_finished(&parser->callbacks.context);
}

static void jsonlite_finish_parse(jsonlite_parser parser) {
    if (parser->result == jsonlite_result_suspended) {
        return;
    }
    
    if (parser->top < parser->stack) {
        return;
    }
    
    parser->tail_size = parser->limit - parser->token_start;
    if (parser->tail_size > 0) {
        parser->tail = malloc(parser->tail_size);
        memcpy(parser->tail, parser->token_start, parser->tail_size); // LCOV_EXCL_LINE
    }
}

static int skip_whitespace(jsonlite_parser parser) {
    const uint8_t *c = parser->cursor;
    const uint8_t *l = parser->limit;
    
    while (unlikely(c < l) && likely(*c == ' ' || *c == '\n' || *c == '\r' || *c == '\t')) {
        ++c;
    }
    
    parser->cursor = c;
    parser->token_start = c;
    return c < l;
}

JSONLITE_FCS set_error(jsonlite_parser parser, const uint8_t *pos, jsonlite_result result) {
    parser->error_pos = pos;
    parser->cursor = pos;
    parser->result = result;
    return 0;
}

JSONLITE_FCS take_object_or_array(jsonlite_parser parser, jsonlite_parse_state jt) {
    switch (*parser->cursor) {
        case '{':
            parser->cursor++;
            jt->function = &take_object_key_end;
            CALL_STATE_CALLBACK(parser->callbacks, object_start);
            return 0;
        case '[':
            parser->cursor++;
            jt->function = &take_array_value_end;
            CALL_STATE_CALLBACK(parser->callbacks, array_start);
            return 0;
        default:
            return set_error(parser, parser->cursor, jsonlite_result_expected_object_or_array);
    }
}

JSONLITE_FCS take_value(jsonlite_parser parser, jsonlite_parse_state jt) {
    uint8_t ch = *parser->cursor;
    if (ch == '-' || (ch >= '0' && ch <= '9')) {
        return take_number(parser);
    }
    
    switch (ch) {
        case '"':
            return take_string(parser);
        case 't':
            return take_true(parser);
        case 'f':
            return take_false(parser);
        case 'n':
            return take_null(parser);
        case '{':
            parser->cursor++;
            jt->function = &take_object_key_end;
            CALL_STATE_CALLBACK(parser->callbacks, object_start);
            return 0;
        case '[':
            parser->cursor++;
            jt->function = &take_array_value_end;
            CALL_STATE_CALLBACK(parser->callbacks, array_start);
            return 0;
        default:
            return set_error(parser, parser->cursor, jsonlite_result_expected_value);
    }
}

JSONLITE_FCS take_object_key_end(jsonlite_parser parser, jsonlite_parse_state jt) {
    switch (*parser->cursor) {
        case '"':
            jt[0].function = &take_object_colon;
            jt[1].function = &take_key;
            return 1;
        case '}':
            parser->cursor++;
            CALL_STATE_CALLBACK(parser->callbacks, object_end);
            return -1;
        default:
            return set_error(parser, parser->cursor, jsonlite_result_expected_key_or_end);
    }
}

JSONLITE_FCS take_object_key(jsonlite_parser parser, jsonlite_parse_state jt) {
    switch (*parser->cursor) {
        case '"':
            jt[0].function = &take_object_colon;
            jt[1].function = &take_key;
            return 1;
        default:
            return set_error(parser, parser->cursor, jsonlite_result_expected_key);
    }
}

JSONLITE_FCS take_object_colon(jsonlite_parser parser, jsonlite_parse_state jt) {
    switch (*parser->cursor++) {
        case ':':
            jt[0].function = &take_object_comma_end;
            jt[1].function = &take_value;
            return 1;
        default:
            return set_error(parser, parser->cursor, jsonlite_result_expected_colon);
    }
}

JSONLITE_FCS take_object_comma_end(jsonlite_parser parser, jsonlite_parse_state jt) {
    switch (*parser->cursor++) {
        case ',':
            jt->function = take_object_key;
            return 0;
        case '}':
            CALL_STATE_CALLBACK(parser->callbacks, object_end);
            return -1;
        default:
            return set_error(parser, parser->cursor, jsonlite_result_expected_comma_or_end);
    }
}

JSONLITE_FCS take_array_value_end(jsonlite_parser parser, jsonlite_parse_state jt) {
    switch (*parser->cursor) {
        case ']':
            parser->cursor++;
            CALL_STATE_CALLBACK(parser->callbacks, array_end);
            return -1;
        default:
            jt[0].function = &take_array_comma_end;
            jt[1].function = &take_value;
            return 1;
    }
}

JSONLITE_FCS take_array_comma_end(jsonlite_parser parser, jsonlite_parse_state jt) {
    switch (*parser->cursor++) {
        case ',':
            jt[1].function = &take_value;
            return 1;
        case ']':
            CALL_STATE_CALLBACK(parser->callbacks, array_end);
            return -1;
        default:
            return set_error(parser, parser->cursor, jsonlite_result_expected_comma_or_end);
    }
}

static jsonlite_result take_string_token(jsonlite_parser parser, jsonlite_value_callback callback) {
    const uint8_t *c = parser->cursor;
    const uint8_t *l = parser->limit;
    jsonlite_token jt;
    jsonlite_string_type type = jsonlite_string_ascii;
    int res;
	uint32_t value;    
step:
    if (++c == l)       goto end_of_stream;
    if (*c == '"')      goto done;
    if (*c == '\\')     goto escaped;    
    if (*c >= 0x80)     goto utf8;
    if (*c < 0x20)      goto error;
    goto step;   
escaped:
    type |= jsonlite_string_escape;
    if (++c == l)       goto end_of_stream;
    switch (*c) {
		case 34: goto step;
		case 47: goto step;
		case 92: goto step;
		case 98: goto step;
		case 102: goto step;
		case 110: goto step;
		case 114: goto step;
		case 116: goto step;
		case 117: goto hex;
	}
    goto error_escape;
hex:
    type |= jsonlite_string_unicode_escape;
    if (++c + 3 >= l)           goto end_of_stream;
    CHECK_HEX(*c);
    value = jsonlite_hex_char_to_uint8(*c++);
    CHECK_HEX(*c);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
    CHECK_HEX(*c);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
    CHECK_HEX(*c);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c);
    if (value < 0xD800 || value > 0xDBFF) goto step;
    if (++c + 5 >= l)     goto end_of_stream;
    if (*c++ != '\\')   goto error_escape;
    if (*c++ != 'u')    goto error_escape;
    CHECK_HEX(*c);
    value = jsonlite_hex_char_to_uint8(*c++);
    CHECK_HEX(*c);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
    CHECK_HEX(*c);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
    CHECK_HEX(*c);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c);
    if (value < 0xDC00 || value > 0xDFFF) goto error_escape;
    goto step;
utf8:
    res = jsonlite_clz(((*c) ^ 0xFF) << 0x19);
    value = 0xAAAAAAAA; // == 1010...
    CHECK_LIMIT(c + res, l);
    switch (res) {
        case 3: value = (value << 2) | (*++c >> 6);
        case 2: value = (value << 2) | (*++c >> 6);
        case 1: value = (value << 2) | (*++c >> 6);
            if (value != 0xAAAAAAAA) {
                return set_error(parser, c - res, jsonlite_result_invalid_utf8);
            }
            break;
        default:
            return set_error(parser, c, jsonlite_result_invalid_utf8);
    }
    goto step;
error_escape:
    return set_error(parser, c, jsonlite_result_invalid_escape);
error:
    return set_error(parser, c, jsonlite_result_invalid_token);
end_of_stream:
    parser->cursor = c;
    return set_error(parser, c, jsonlite_result_end_of_stream);
done:
    jt.string_type = type;
    jt.start = parser->cursor + 1;
    jt.end = c;
    parser->cursor = c + 1;
    callback(&parser->callbacks.context, &jt);
    return -1;
}

JSONLITE_FCS take_key(jsonlite_parser parser, jsonlite_parse_state ps) {
    return take_string_token(parser, parser->callbacks.key_found);
}

JSONLITE_FCS take_string(jsonlite_parser parser) {
    return take_string_token(parser, parser->callbacks.string_found);
}

JSONLITE_FCS take_number(jsonlite_parser parser) {
    const uint8_t *c = parser->cursor;
    const uint8_t *l = parser->limit;
    jsonlite_token jt;
    jsonlite_number_type type = jsonlite_number_int;
    
    switch (*c) {
        case 45: type |= jsonlite_number_negative;      goto test_zero_leading;
        case 48: type |= jsonlite_number_zero_leading;  goto take_exp_frac;
        default:                                        goto take_digits;
    }    

test_zero_leading:
    if (++c == l)               goto end_of_stream;
    if (49 <= *c && *c <= 57)   goto take_digits;
    if (*c == 48)               goto take_exp_frac;
    goto parse_error;
take_exp_frac:
    if (++c == l)               goto end_of_stream;    
    switch (*c) {
        case 9:                 goto found_token;
        case 10:                goto found_token;
        case 13:                goto found_token;
        case 32:                goto found_token;
        case 44:                goto found_token;
        case 46:                goto found_fraction;
        case 69:                goto take_exponent;
        case 93:                goto found_token;
        case 101:               goto take_exponent;
        case 125:               goto found_token;
        default:                goto parse_error;
    }
found_fraction:
    type |= jsonlite_number_frac;
    if (++c == l)               goto end_of_stream;    
    if (48 <= *c && *c <= 57)   goto take_frac_number;
    goto parse_error;
take_frac_number:
    if (++c == l)               goto end_of_stream;
    if (48 <= *c && *c <= 57)   goto take_frac_number;
    switch (*c) {
        case 9:                 goto found_token;
        case 10:                goto found_token;
        case 13:                goto found_token;
        case 32:                goto found_token;
        case 44:                goto found_token;
        case 69:                goto take_exponent;
        case 93:                goto found_token;
        case 101:               goto take_exponent;
        case 125:               goto found_token;
        default:                goto parse_error;
    }
take_exponent:
    type |= jsonlite_number_exp;
    if (++c == l)               goto end_of_stream;
    if (48 <= *c && *c <= 57)   goto take_exponent_number;
    switch(*c) {
        case 43:                goto take_exponent_sign;
        case 45:                goto take_exponent_sign;
        default:                goto parse_error;
    }
take_exponent_sign:
    if (++c == l)               goto end_of_stream;
    if (48 <= *c && *c <= 57)   goto take_exponent_number;
    goto parse_error;
take_exponent_number:
    if (++c == l)               goto end_of_stream;
    if (48 <= *c && *c <= 57)   goto take_exponent_number;
    switch(*c) {
        case 9:                 goto found_token;
        case 10:                goto found_token;
        case 13:                goto found_token;
        case 32:                goto found_token;
        case 44:                goto found_token;
        case 93:                goto found_token;
        case 125:               goto found_token;
        default:                goto parse_error;
    }
take_digits:
    if (++c == l)               goto end_of_stream;
    if (48 <= *c && *c <= 57)   goto take_digits;
    switch(*c) {
        case 9:                 goto found_token;
        case 10:                goto found_token;
        case 13:                goto found_token;
        case 32:                goto found_token;
        case 44:                goto found_token;
        case 46:                goto found_fraction;
        case 69:                goto take_exponent;
        case 93:                goto found_token;
        case 101:               goto take_exponent;
        case 125:               goto found_token;
        default:                goto parse_error;
    }
parse_error:
    return set_error(parser, c, jsonlite_result_invalid_number);
end_of_stream:
    parser->cursor = c;
    return set_error(parser, c, jsonlite_result_end_of_stream);
found_token:
    jt.start = parser->cursor;
    jt.end = c;
    jt.number_type = type;
    parser->cursor = c;
    CALL_VALUE_CALLBACK(parser->callbacks, number_found, &jt);
    return -1;
}

JSONLITE_FCS take_true(jsonlite_parser parser) {
    const uint8_t *c = parser->cursor + 1;
    CHECK_LIMIT(c + 2, parser->limit);
    if (*c++ != 'r') goto error;
    if (*c++ != 'u') goto error;
    if (*c != 'e') goto error;
    parser->cursor += 4;
    CALL_STATE_CALLBACK(parser->callbacks, true_found);
    return -1;
error:
    return set_error(parser, parser->cursor, jsonlite_result_invalid_token);
}

JSONLITE_FCS take_false(jsonlite_parser parser) {
    const uint8_t *c = parser->cursor + 1;
    CHECK_LIMIT(c + 3, parser->limit);
    if (*c++ != 'a') goto error;
    if (*c++ != 'l') goto error;
    if (*c++ != 's') goto error;
    if (*c != 'e') goto error;
    parser->cursor += 5;
    CALL_STATE_CALLBACK(parser->callbacks, false_found);
    return -1;
error:
    return set_error(parser, parser->cursor, jsonlite_result_invalid_token);
}

JSONLITE_FCS take_null(jsonlite_parser parser) {
    const uint8_t *c = parser->cursor + 1;
    CHECK_LIMIT(c + 2, parser->limit);
    if (*c++ != 'u') goto error;
    if (*c++ != 'l') goto error;
    if (*c != 'l') goto error;
    parser->cursor += 4;
    CALL_STATE_CALLBACK(parser->callbacks, null_found);
    return -1;
error:
    return set_error(parser, parser->cursor, jsonlite_result_invalid_token);
}
