#ifndef __CHORD_MARCO_H__
#define __CHORD_MARCO_H__

#include <string.h>
#include <assert.h> //断言
#include "util.h"

#define CHORD_ASSERT(x) \
    if(!(x)) { \
        CHORD_LOG_ERROR(CHORD_LOG_ROOT()) << "ASSERTION: " << #x \
            << "\nbacktrace:\n" \
            << chord::BacktraceToString(20, 2, "    "); \
        assert(x); \
    }

#define CHORD_ASSERT2(x, w) \
    if(!(x)) { \
        CHORD_LOG_ERROR(CHORD_LOG_ROOT()) << "ASSERTION: " << #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << chord::BacktraceToString(20, 2, "    "); \
        assert(x); \
    }

#endif