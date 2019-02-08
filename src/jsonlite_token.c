#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_token.h"
#include "jsonlite_utf.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdlib.h>

static uint8_t jsonlite_hex_char_to_uint8(uint8_t c) {
    jsonlite_stack_check();
    if (c >= 'a') {
        jsonlite_stack_check();
        return (uint8_t)(c - 'a' + 10);
    }

    if (c >= 'A') {
        jsonlite_stack_check();
        return (uint8_t)(c - 'A' + 10);
    }

    jsonlite_stack_check();
    return (uint8_t)(c - '0');
}

static int unicode_char_to_utf16(uint32_t utf32, uint16_t *utf16) {
    jsonlite_stack_check();
    uint32_t v = utf32 - 0x10000;
    uint32_t vh = v >> 10;
    uint32_t vl = v & 0x3FF;
	if (utf32 <= 0xFFFF) {
        *utf16 = (uint16_t)utf32;
        jsonlite_stack_check();
        return 1;
    }

    *utf16++ = (uint16_t)(0xD800 + vh);
    *utf16 = (uint16_t)(0xDC00 + vl);
    jsonlite_stack_check();
    return 2;
}

size_t jsonlite_token_size_of_uft8(jsonlite_token *ts) {
    return (size_t)(ts->end - ts->start + 1);
}

size_t jsonlite_token_to_uft8(jsonlite_token *ts, void *buffer) {
    jsonlite_stack_check();
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

    jsonlite_stack_check();
    return c - (uint8_t *)buffer;
}

size_t jsonlite_token_size_of_uft16(jsonlite_token *ts) {
    jsonlite_stack_check();
    return (ts->end - ts->start + 1) * sizeof(uint16_t);
}

size_t jsonlite_token_to_uft16(jsonlite_token *ts, void *buffer) {
    jsonlite_stack_check();

    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
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
    res = jsonlite_utf8_sequence_length(*p);
    uint32_t code = (uint32_t)(*p & (0xFF >> (res + 1)));
    switch (res) {
        case 4: code = (code << 6) | (*++p & 0x3F);
        case 3: code = (code << 6) | (*++p & 0x3F);
        default: code = (code << 6) | (*++p & 0x3F), ++p;
    }

    c += unicode_char_to_utf16(code, c);
    goto step;
done:
    *c = 0;
    jsonlite_stack_check();
    return (c - (uint16_t *)buffer) * sizeof(uint16_t);
}

size_t jsonlite_token_size_of_base64_binary(jsonlite_token *ts) {
    jsonlite_stack_check();
    return (((ts->end - ts->start) * 3) / 4 + 3) & ~3;
}

size_t jsonlite_token_base64_to_binary(jsonlite_token *ts, void *buffer) {
    jsonlite_stack_check();
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
    jsonlite_stack_check();
    return length;
}

long jsonlite_token_to_long(jsonlite_token *token) {
    jsonlite_stack_check();
    long res = 0;
    int negative = (token->type & jsonlite_number_negative) == jsonlite_number_negative;
    const uint8_t *c = token->start + negative;
    const uint8_t *e = token->end;
    while (c != e) {
        res = res * 10 + (*c++ - '0');
    }

    jsonlite_stack_check();
    return negative ? -res : res;
}

long long jsonlite_token_to_long_long(jsonlite_token *token) {
    jsonlite_stack_check();
    long long res = 0;
    int negative = (token->type & jsonlite_number_negative) == jsonlite_number_negative;
    const uint8_t *c = token->start + negative;
    const uint8_t *e = token->end;
    while (c != e) {
        res = res * 10 + (*c++ - '0');
    }

    jsonlite_stack_check();
    return negative ? -res : res;
}
