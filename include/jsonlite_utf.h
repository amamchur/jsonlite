#ifndef JSONLITE_JSONLITE_UTF_H
#define JSONLITE_JSONLITE_UTF_H

#include <stdint.h>

#ifdef _MSC_VER

#include <intrin.h>

static uint32_t jsonlite_utf8_sequence_length(uint8_t c) {
    unsigned long r = 0;
    unsigned long x = c;
    _BitScanForward(&r, x);
    return r;
}

#else

static int jsonlite_utf8_sequence_length(uint8_t c) {
    unsigned int value = c;
    unsigned int inverted = ~value;
    unsigned int shift = sizeof(unsigned int) * 8 - 8;
    unsigned int leading_zeros = inverted << shift;
    return __builtin_clz(leading_zeros);
}

#define jsonlite_clz(x) __builtin_clz((x))

#endif

#endif
