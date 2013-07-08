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

static uint32_t __inline jsonlite_clz( uint32_t x ) {
   unsigned long r = 0;
   _BitScanForward(&r, x);
   return r;
}

#else

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

struct jsonlite_parse_state_struct;

typedef struct jsonlite_parse_state_struct* jsonlite_parse_state;
typedef int (*jsonlite_parse_function)(jsonlite_parser, jsonlite_parse_state);

typedef struct jsonlite_parse_state_struct {
    jsonlite_parse_function function;
} jsonlite_parse_state_struct;

typedef enum parse_state {
    state_start,
    state_take_object_key,
    state_take_object_key_end,
    state_take_object_colon,
    state_take_object_comma_end,
    state_take_array_value_end,
    state_take_array_comma_end,
    state_take_key,
    state_take_value,
    state_finish
} parse_state;

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
    
    parse_state *current;
    parse_state *states;
    jsonlite_parser_callbacks callbacks;
} jsonlite_parser_struct;

static void jsonlite_do_parse(jsonlite_parser parser);
static void jsonlite_finish_parse(jsonlite_parser parser);

JSONLITE_FCS set_error(jsonlite_parser parser, const uint8_t *pos, jsonlite_result result);
static const uint8_t* take_number(jsonlite_parser parser, const uint8_t *c, const uint8_t *l);

static int skip_whitespace(jsonlite_parser parser);
static const uint8_t* take_string_token(jsonlite_parser parser, jsonlite_value_callback callback, const uint8_t *c, const uint8_t *l);

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
    depth = depth < 4 ? 32 : depth;
    return sizeof(jsonlite_parser_struct) + depth * sizeof(parse_state);
}

jsonlite_parser jsonlite_parser_init(size_t depth) {
    jsonlite_parser parser;
    depth = depth < 4 ? 32 : depth;
    
    parser = (jsonlite_parser)malloc(jsonlite_parser_estimate_size(depth));
    parser->cursor = NULL;
    parser->limit = NULL;
    parser->token_start = NULL;
    parser->buffer = NULL;
    parser->buffer_own = NULL;
    parser->tail = NULL;
    parser->tail_size = 0;
    parser->result = jsonlite_result_unknown;
    parser->depth = depth;
    parser->callbacks = jsonlite_default_callbacks;
    parser->error_pos = NULL;
    parser->states = (parse_state *)((uint8_t *)parser + sizeof(jsonlite_parser_struct));
    parser->states[0] = state_finish;
    parser->states[1] = state_start;
    parser->current = parser->states + 1;
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
    const uint8_t *c = parser->cursor;
    const uint8_t *l = parser->limit;
    const uint8_t *token_start = NULL;
    parse_state *last = parser->states + parser->depth - 1;
    parse_state *state = parser->current;    
    parser->result = jsonlite_result_unknown;
    goto next_state;
    
object_or_array_finished:
    state--;
    if (*state == state_finish) goto success;    
skip_char_and_space:
    c++;
next_state:
    if (c == l)     goto end_of_stream_space;
    if (*c == ' ')  goto skip_char_and_space;
    if (*c == '\n') goto skip_char_and_space;
    if (*c == '\r') goto skip_char_and_space;
    if (*c == '\t') goto skip_char_and_space;
    token_start = c;
    
    if (parser->result != jsonlite_result_unknown) goto finished;
    switch (*state) {
        case state_start:                   goto root_object_detection;
        case state_take_object_key:         goto take_object_key;
        case state_take_object_key_end:     goto take_object_key_end;
        case state_take_object_colon:       goto take_object_colon;
        case state_take_object_comma_end:   goto take_object_comma_end;
        case state_take_array_value_end:    goto take_array_value_end;
        case state_take_array_comma_end:    goto take_array_comma_end;
        case state_take_key:                goto key_parsing;
        case state_take_value:              goto value_type_detection;
        case state_finish:                  goto success;
    }
    
root_object_detection:
    if (*c == '{') goto object_state_machine;
    if (*c == '[') goto array_state_machine;
    parser->result = jsonlite_result_expected_object_or_array;
    goto finished;
    
object_state_machine:
    *state = state_take_object_key_end;
    CALL_STATE_CALLBACK(parser->callbacks, object_start);
    goto skip_char_and_space;
array_state_machine:
    *state = state_take_array_value_end;
    CALL_STATE_CALLBACK(parser->callbacks, array_start);
    goto skip_char_and_space;
    
take_object_key_end:
    switch (*c) {
        case '"':
            *state = state_take_object_colon;
            if (++state == last) goto depth_limit;
            *state = state_take_key;
            goto key_parsing;
        case '}':
            CALL_STATE_CALLBACK(parser->callbacks, object_end);
            goto object_or_array_finished;
        default:
            parser->result = jsonlite_result_expected_key_or_end;
            goto finished;
    }
take_object_comma_end:
    switch (*c) {
        case ',':
            *state = state_take_object_key;
            goto skip_char_and_space;
        case '}':
            CALL_STATE_CALLBACK(parser->callbacks, object_end);
            goto object_or_array_finished;
        default:
            parser->result = jsonlite_result_expected_comma_or_end;
            goto finished;
    }
take_object_key:
    switch (*c) {
        case '"':
            *state = state_take_object_colon;
            if (++state == last) goto depth_limit;
            *state = state_take_key;
            goto key_parsing;
        default:
            parser->result = jsonlite_result_expected_key;
            goto finished;
    }
take_object_colon:
    switch (*c) {
        case ':':
            *state = state_take_object_comma_end;
            if (++state == last) goto depth_limit;
            *state = state_take_value;
            goto skip_char_and_space;
        default:
            parser->result = jsonlite_result_expected_colon;
            goto finished;
    }

take_array_value_end:
    switch (*c) {
        case ']':
            CALL_STATE_CALLBACK(parser->callbacks, array_end);
            goto object_or_array_finished;
        default:
            *state = state_take_array_comma_end;
            if (++state == last) goto depth_limit;
            *state = state_take_value;
            goto next_state;
    }
take_array_comma_end:
    switch (*c) {
        case ',':
            if (++state == last) goto depth_limit;
            *state = state_take_value;
            goto skip_char_and_space;
        case ']':
            CALL_STATE_CALLBACK(parser->callbacks, array_end);
            goto object_or_array_finished;
        default:
            parser->result = jsonlite_result_expected_comma_or_end;
            goto finished;
    }
    
value_type_detection:
    if (*c == '-' || (*c >= '0' && *c <= '9'))  goto number_parsing;
    if (*c == '"')                              goto string_parsing;
    if (*c == 't')                              goto true_token_parsing;
    if (*c == 'f')                              goto false_token_paring;
    if (*c == 'n')                              goto null_token_parsing;
    if (*c == '{')                              goto object_state_machine;
    if (*c == '[')                              goto array_state_machine;
    parser->result = jsonlite_result_expected_value;
    goto finished;
    
number_parsing:
    c = take_number(parser, c, l);
    goto check_error;
key_parsing:
    c = take_string_token(parser, parser->callbacks.key_found, c, l);
    goto check_error;
string_parsing:
    c = take_string_token(parser, parser->callbacks.string_found, c, l);
    goto check_error;
    
true_token_parsing:
    if (++c + 2 >= l)   goto end_of_stream;
    if (*c++ != 'r')    goto invalid_token;
    if (*c++ != 'u')    goto invalid_token;
    if (*c++ != 'e')    goto invalid_token;
    CALL_STATE_CALLBACK(parser->callbacks, true_found);
    state--;
    goto next_state;
false_token_paring:
    if (++c + 3 >= l)   goto end_of_stream;
    if (*c++ != 'a')    goto invalid_token;
    if (*c++ != 'l')    goto invalid_token;
    if (*c++ != 's')    goto invalid_token;
    if (*c++ != 'e')    goto invalid_token;
    CALL_STATE_CALLBACK(parser->callbacks, false_found);
    state--;
    goto next_state;
null_token_parsing:
    if (++c + 2 >= l)   goto end_of_stream;
    if (*c++ != 'u')    goto invalid_token;
    if (*c++ != 'l')    goto invalid_token;
    if (*c++ != 'l')    goto invalid_token;
    CALL_STATE_CALLBACK(parser->callbacks, null_found);
    state--;
    goto next_state;
    
check_error:
    if (parser->result != jsonlite_result_unknown && parser->result != jsonlite_result_suspended)      goto finished;
    state--;
    goto next_state;

invalid_token:
    parser->result = jsonlite_result_invalid_token;
    goto finished;
depth_limit:
    parser->result = jsonlite_result_depth_limit;
    goto finished;
    
end_of_stream_space:
    token_start = l;
end_of_stream:
    parser->result = jsonlite_result_end_of_stream;
    goto finished;
    
success:
    parser->result = jsonlite_result_ok;
finished:
    parser->token_start = token_start;
    parser->current = state;
    parser->cursor = c;
    parser->callbacks.parse_finished(&parser->callbacks.context);
}

static void jsonlite_finish_parse(jsonlite_parser parser) {
    if (parser->result == jsonlite_result_suspended) {
        return;
    }
    
    if (*parser->current == state_finish) {
        return;
    }
    
    parser->tail_size = parser->limit - parser->token_start;
    if (parser->tail_size > 0) {
        parser->tail = malloc(parser->tail_size);
        memcpy(parser->tail, parser->token_start, parser->tail_size); // LCOV_EXCL_LINE
    }
}

JSONLITE_FCS set_error(jsonlite_parser parser, const uint8_t *pos, jsonlite_result result) {
    parser->error_pos = pos;
    parser->cursor = pos;
    parser->result = result;
    return 0;
}

static const uint8_t* take_string_token(jsonlite_parser parser, jsonlite_value_callback callback, const uint8_t *c, const uint8_t *l) {
    jsonlite_token jt;
    jsonlite_string_type type = jsonlite_string_ascii;
    int res;
	uint32_t value, utf32;
    jt.start = c + 1;
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
    if ((value & 0xFFFFu) >= 0xFFFEu)           goto error;
    if (value >= 0xFDD0u && value <= 0xFDEFu)   goto error;
    if (0xD800 > value || value > 0xDBFF)       goto step;
    
    // UTF-16 Surrogate
    utf32 = (value - 0xD800) << 10;
    if (++c + 5 >= l)                           goto end_of_stream;
    if (*c++ != '\\')                           goto error_escape;
    if (*c++ != 'u')                            goto error_escape;
    CHECK_HEX(*c);
    value = jsonlite_hex_char_to_uint8(*c++);
    CHECK_HEX(*c);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
    CHECK_HEX(*c);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c++);
    CHECK_HEX(*c);
    value = (uint32_t)(value << 4) | jsonlite_hex_char_to_uint8(*c);
    if (value < 0xDC00 || value > 0xDFFF)       goto error_escape;
    utf32 += value - 0xDC00 + 0x10000;
    if ((utf32 & 0x0FFFFu) >= 0x0FFFEu)         goto error;
    goto step;
utf8:
    res = jsonlite_clz(((*c) ^ 0xFF) << 0x19);
    utf32 = (*c & (0xFF >> (res + 1)));
    value = 0xAAAAAAAA; // == 1010...
    CHECK_LIMIT(c + res, l);
    switch (res) {
        case 3: value = (value << 2) | (*++c >> 6); utf32 = (utf32 << 6) | (*c & 0x3F);
        case 2: value = (value << 2) | (*++c >> 6); utf32 = (utf32 << 6) | (*c & 0x3F);
        case 1: value = (value << 2) | (*++c >> 6); utf32 = (utf32 << 6) | (*c & 0x3F);
            if (value != 0xAAAAAAAA)                    goto error_utf8;
            if ((utf32 & 0xFFFFu) >= 0xFFFEu)           goto error_utf8;
            if (utf32 >= 0xFDD0u && utf32 <= 0xFDEFu)   goto error_utf8;
    }
    goto step;
error_utf8:
    set_error(parser, c - res, jsonlite_result_invalid_utf8);
    return c;
error_escape:
    set_error(parser, c, jsonlite_result_invalid_escape);
    return c;
error:
    set_error(parser, c, jsonlite_result_invalid_token);
    return c;
end_of_stream:
    parser->cursor = c;
    return set_error(parser, c, jsonlite_result_end_of_stream);
done:
    jt.string_type = type;
    jt.end = c;
    parser->cursor = c + 1;
    callback(&parser->callbacks.context, &jt);
    return c + 1;
}

static const uint8_t* take_number(jsonlite_parser parser, const uint8_t *c, const uint8_t *l) {
    jsonlite_token jt;
    jsonlite_number_type type = jsonlite_number_int;
    jt.start = c;
    
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
    set_error(parser, c, jsonlite_result_invalid_number);
    return c;
end_of_stream:
    set_error(parser, c, jsonlite_result_end_of_stream);
    return c;
found_token:
    jt.end = c;
    jt.number_type = type;
    CALL_VALUE_CALLBACK(parser->callbacks, number_found, &jt);
    return c;
}
