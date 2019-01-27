//
//  Copyright 2012-2019, Andrii Mamchur
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
#include "jsonlite_utf.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <jsonlite_parser.h>

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
static void empty_value_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {}
static void empty_state_callback(jsonlite_callback_context *ctx, jsonlite_event event) {}

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

// Number parsing
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

// String parsing
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
        case 0x22:      goto next_char;     // "    quotation mark  U+0022
        case 0x2F:      goto next_char;     // /    solidus         U+002F
        case 0x5C:      goto next_char;     // \    reverse solidus U+005C
        case 0x62:      goto next_char;     // b    backspace       U+0008
        case 0x66:      goto next_char;     // f    form feed       U+000C
        case 0x6E:      goto next_char;     // n    line feed       U+000A
        case 0x72:      goto next_char;     // r    carriage return U+000D
        case 0x74:      goto next_char;     // t    tab             U+0009
        case 0x75:      goto hex;           // uXXXX                U+XXXX
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

    // UTF-16 Surrogate
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
    // See https://en.wikipedia.org/wiki/UTF-8#Description
    // 0xxxxxxx -> ASCII char, should not occur here, checked in next_char state
    // 10xxxxxx -> error
    // 110xxxxx -> sequence length 2
    // 1110xxxx -> sequence length 3
    // 11110xxx -> sequence length 4
    // 11111xxx -> error
    int res = jsonlite_utf8_sequence_length(*c);
    if (2 > res || res > 4) goto error_utf8;
    if (c + res >= l) goto end_of_stream;
    utf32 = (uint32_t)(*c & (0xFF >> (res)));
    value = 0xAAAAAAAA; // == 1010...
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

// Primitive tokens
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

// Error states.
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

// End of stream states.
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
