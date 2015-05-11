#ifndef CC_FRONTEND_FRONTEND_RESULT_H_
#define CC_FRONTEND_FRONTEND_RESULT_H_

#include <unordered_map>
#include <vector>

#include "cc/base/unicode.h"
#include "cc/frontend/base/chr_index.h"
#include "cc/frontend/base/token.h"
#include "cc/frontend/base/token_index.h"
#include "cc/frontend/base/token_span.h"

using std::unordered_map;
using std::vector;

struct FrontendResult {
    // The original Unicode string.
    ustring text;

    // Destuttered, HTMl-parsed text after Unicode normalization.
    ustring clean_text;
    vector<ChrIndex> clean2original;
    unordered_map<uchar, size_t> chr2drop;

    // Tokens after ASCII normalization and tokenization.
    vector<string> prenormalized_tokens;
    vector<TokenSpan> token2clean;

    // Tagged, unreducted, respelled tokens after token normalization.
    vector<Token> tokens;
    vector<TokenIndex> token2prenormalized;
};

#endif  // CC_FRONTEND_FRONTEND_RESULT_H_
