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
#include "../include/jsonlite_token.h"
#endif

#include <stdlib.h>

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

static uint8_t jsonlite_hex_char_to_uint8(uint8_t c) {
    if (c >= 'a') {
        return (uint8_t)(c - 'a' + 10);
    }
    
    if (c >= 'A') {
        return (uint8_t)(c - 'A' + 10);
    }
    
    return (uint8_t)(c - '0');
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

size_t jsonlite_token_size_of_uft8(jsonlite_token *ts) {
    return (size_t)(ts->end - ts->start + 1);
}

size_t jsonlite_token_to_uft8(jsonlite_token *ts, uint8_t *buffer) {
    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
    uint32_t value, utf32;
  	uint8_t *c = buffer;
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
    res = jsonlite_clz(((*p) ^ 0xFF) << 0x19);
    *c++ = *p++;
    switch (res) {
        case 3: *c++ = *p++;
        case 2: *c++ = *p++;
        case 1: *c++ = *p++;
        default: break;
    }
    goto step;
done:
    *c = 0;
    return c - buffer;
}

size_t jsonlite_token_size_of_uft16(jsonlite_token *ts) {
    return (ts->end - ts->start + 1) * sizeof(uint16_t);
}

size_t jsonlite_token_to_uft16(jsonlite_token *ts, uint16_t *buffer) {
    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
    uint16_t utf16;
    uint16_t *c = buffer;
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
    res = jsonlite_clz(((*p) ^ 0xFF) << 0x19);
    uint32_t code = (uint32_t)(*p & (0xFF >> (res + 1)));
    switch (res) {
        case 3: code = (code << 6) | (*++p & 0x3F);
        case 2: code = (code << 6) | (*++p & 0x3F);
        case 1: code = (code << 6) | (*++p & 0x3F);
        case 0: ++p;
        default: break;
    }
    
    c += unicode_char_to_utf16(code, c);
    goto step;
done:
    *c = 0;
    return (c - buffer) * sizeof(uint16_t);
}

size_t jsonlite_token_size_of_base64_binary(jsonlite_token *ts) {
    return (((ts->end - ts->start) * 3) / 4 + 3) & ~3;
}

size_t jsonlite_token_base64_to_binary(jsonlite_token *ts, void *buffer) {
    size_t length = 0;
    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
    uint8_t *c = buffer;
    size_t bytes, i;
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
    int negative = (token->type.number & jsonlite_number_negative) == jsonlite_number_negative;
    ptrdiff_t length = token->end - token->start - negative;
    const uint8_t *c = token->start + negative;
    switch (length & 3) {
        for (; length > 0; length -= 4) {
            case 0: res = res * 10 + *c++ - '0';
            case 3: res = res * 10 + *c++ - '0';
            case 2: res = res * 10 + *c++ - '0';
            case 1: res = res * 10 + *c++ - '0';
        }
    }
    
    return negative ? -res : res;
}

long long jsonlite_token_to_long_long(jsonlite_token *token) {
    long long res = 0;
    int negative = (token->type.number & jsonlite_number_negative) == jsonlite_number_negative;
    ptrdiff_t length = token->end - token->start - negative;
    const uint8_t *c = token->start + negative;
    switch (length & 7) {
        for (; length > 0; length -= 8) {
            case 0: res = res * 10 + *c++ - '0';
            case 7: res = res * 10 + *c++ - '0';
            case 6: res = res * 10 + *c++ - '0';
            case 5: res = res * 10 + *c++ - '0';
            case 4: res = res * 10 + *c++ - '0';
            case 3: res = res * 10 + *c++ - '0';
            case 2: res = res * 10 + *c++ - '0';
            case 1: res = res * 10 + *c++ - '0';
        }
    }
    
    return negative ? -res : res;
}
