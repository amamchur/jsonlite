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

#ifdef JSONLITE_SINGLE_FOLDER_LOCATION
#include "jsonlite_parser.h"
#else
#include "../include/jsonlite_parser.h"
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
