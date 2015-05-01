#ifndef CC_TOKENIZATION_SPAN_H_
#define CC_TOKENIZATION_SPAN_H_

#include <cstdint>

struct Span {
    uint16_t begin;
    uint16_t end_excl;
};

#endif  // CC_TOKENIZATION_SPAN_H_
