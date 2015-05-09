#ifndef CC_FRONTEND_BASE_TOKEN_SPAN_H_
#define CC_FRONTEND_BASE_TOKEN_SPAN_H_

#include "cc/frontend/base/chr_index.h"

struct TokenSpan {
    ChrIndex begin;
    ChrIndex end_excl;
};

#endif  // CC_FRONTEND_BASE_SPAN_H_
