#ifndef JSONLITE_STACK_CHECK_H
#define JSONLITE_STACK_CHECK_H

#include <stddef.h>
#include <stdint.h>

#ifdef JSONLIE_STACK_CHECK

extern ptrdiff_t jsonlite_stack_origin;
extern ptrdiff_t jsonlite_stack_max;
extern ptrdiff_t jsonlite_stack_min;

static void jsonlite_stack_check() {
    uint8_t stack_marker = 0;
    ptrdiff_t current = (ptrdiff_t)&stack_marker;
    jsonlite_stack_max = jsonlite_stack_max < current ? current : jsonlite_stack_max;
    jsonlite_stack_min = jsonlite_stack_min > current ? current : jsonlite_stack_min;
}

static void jsonlite_stack_check_init() {
    uint8_t stack_marker = 0;
    jsonlite_stack_origin = (ptrdiff_t)&stack_marker;
    jsonlite_stack_max = PTRDIFF_MIN;
    jsonlite_stack_min = PTRDIFF_MAX;
}

static ptrdiff_t jsonlite_stack_used() {
    ptrdiff_t a = jsonlite_stack_origin - jsonlite_stack_max;
    ptrdiff_t b = jsonlite_stack_origin - jsonlite_stack_min;
    if (a < 0) {
        a = -a;
    }

    if (b < 0) {
        b = -b;
    }

    return a < b ? b : a;
}

#else

#define jsonlite_stack_check()
#define jsonlite_stack_check_init()
#define jsonlite_stack_used() PTRDIFF_MAX

#endif

#endif
