//
//  Copyright 2012, Andrii Mamchur
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

#include "../include/jsonlite_token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static const uint32_t jsonlite_utf8_masks[] = {0x1F, 0x7FF, 0xFFFF, 0x1FFFFF};

uint8_t jsonlite_hex_char_to_uint8(uint8_t c) {
    uint8_t res;
    if (c >= '0' && c <= '9') {
        res = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        res = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        res = c - 'A' + 10;
    } else {
        res = 0xFF;
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
    const uint8_t *p = ts->start;
  	uint8_t *c;
	uint32_t hex;
    if (*buffer == NULL) {
        *buffer = (uint8_t *)malloc(jsonlite_token_decode_size_for_uft8(ts));
    }
    
    c = *buffer;

    while (p < ts->end) {
        if (*p != '\\') {
            *c++ = *p++;
            continue;
        }
        switch (*++p) {
            case '"':
                *c++ = '"';
                break;
            case '\\':
                *c++ = '\\';
                break;
            case '/':
                *c++ = '/';
                break;
            case 'b':
                *c++ = '\b';
                break;
            case 'f':
                *c++ = '\f';
                break;
            case 'n':
                *c++ = '\n';
                break;
            case 'r':
                *c++ = '\r';
                break;
            case 't':
                *c++ = '\t';
                break;
            case 'u': {
                p++;
                hex = jsonlite_hex_char_to_uint8(*p++);
                hex = hex << 4 | jsonlite_hex_char_to_uint8(*p++);
                hex = hex << 4 | jsonlite_hex_char_to_uint8(*p++);
                hex = hex << 4 | jsonlite_hex_char_to_uint8(*p);
                if (hex < 0x80) {
                    *c++ = (uint8_t)hex;
                } else if (hex < 0x0800) {
                    c[1] = (hex & 0x3F) | 0x80;
                    hex = hex >> 6;
                    c[0] = (uint8_t)hex | 0xC0;
                    c += 2;
                } else if (hex <= 0xFFFF) {
                    c[2] = (hex & 0x3F) | 0x80;
                    hex = hex >> 6;
                    c[1] = (hex & 0x3F) | 0x80;
                    hex = hex >> 6;
                    c[0] = (uint8_t)hex | 0xE0;
                    c += 3;
                }
                break;
            }
        }
        p++;
        
    }
    *c = 0;
    return c - *buffer;
}

size_t jsonlite_token_decode_size_for_uft16(jsonlite_token *ts) {
    if (ts == NULL) {
        return 0;
    }
    
    return (ts->end - ts->start) * sizeof(uint16_t) + 2;
}

size_t jsonlite_token_decode_to_uft16(jsonlite_token *ts, uint16_t **buffer) {
    const uint8_t *p = ts->start;
    const uint8_t *l = ts->end;
    uint16_t *c;
    if (*buffer == NULL) {
        *buffer = (uint16_t *)malloc(jsonlite_token_decode_size_for_uft16(ts));
    }
    
    c = *buffer;
    
    for (; p < l;) {
        uint32_t unichar = *p;
        if (unichar == '\\') {
            switch (*++p) {
                case '"':
                    *c = '"';
                    break;
                case '\\':
                    *c = '\\';
                    break;
                case '/':
                    *c = '/';
                    break;
                case 'b':
                    *c = '\b';
                    break;
                case 'f':
                    *c = '\f';
                    break;
                case 'n':
                    *c = '\n';
                    break;
                case 'r':
                    *c = '\r';
                    break;
                case 't':
                    *c = '\t';
                    break;
                case 'u': {
                    uint16_t hex = jsonlite_hex_char_to_uint8(*++p);
                    hex = (uint16_t)(hex << 4) | (uint16_t)jsonlite_hex_char_to_uint8(*++p);
                    hex = (uint16_t)(hex << 4) | (uint16_t)jsonlite_hex_char_to_uint8(*++p);
                    hex = (uint16_t)(hex << 4) | (uint16_t)jsonlite_hex_char_to_uint8(*++p);
                    *c = hex;                
                    break;
                }
            }
            ++c;
            ++p;
        } else if (unichar < 0x80) {
            *c++ = *p++;
        } else {
            int res = jsonlite_clz(((unichar) ^ 0xFF) << 0x19);
            uint32_t code = unichar;
            switch (res) {
                case 3: code = (code << 6) | (*++p & 0x3F);
                case 2: code = (code << 6) | (*++p & 0x3F);
                case 1: code = (code << 6) | (*++p & 0x3F);
                case 0: ++p;
            }
            
            c += unicode_char_to_utf16(code & jsonlite_utf8_masks[res], c);
        }
    }
    *c = 0;
    return (c - *buffer) * sizeof(uint16_t);
}
