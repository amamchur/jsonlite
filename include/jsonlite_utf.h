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
//  See the License for the specific l

#ifndef JSONLITE_JSONLITE_UTF_H
#define JSONLITE_JSONLITE_UTF_H

#ifdef _MSC_VER

#include <intrin.h>

static inline uint32_t __inline jsonlite_utf8_sequence_length(uint8_t c) {
   unsigned long r = 0;
   unsigned long x = c;
   _BitScanForward(&r, x);
   return r;
}

#else

//#define jsonlite_utf8_sequence_length(x) __builtin_clz(((uint32_t)(x) ^ 0xFFu) << 0x19)

static inline int jsonlite_utf8_sequence_length(uint8_t c) {
    unsigned int value = c; // cast uint8_t to unsigned int
    unsigned int inverted = ~value; // invert bits, now we can count leading zeros, instead of one
    unsigned int shift = sizeof(unsigned int) * 8 - 8; // move uint8_t data to begin
    unsigned int leading_zeros = inverted << shift;
    return __builtin_clz(leading_zeros);
}

#define jsonlite_clz(x) __builtin_clz((x))


#endif

#endif
