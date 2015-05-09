#ifndef CC_FRONTEND_FRONTEND_RESULT_H_
#define CC_FRONTEND_FRONTEND_RESULT_H_

#include <unordered_map>
#include <vector>

#include "cc/base/unicode.h"
#include "cc/frontend/base/chr_index.h"
#include "cc/frontend/base/token.h"
#include "cc/frontend/base/token_span.h"

using std::unordered_map;
using std::vector;

struct FrontendResult {
    ustring original_text;

    ustring clean_text;
    vector<ChrIndex> clean2original;
    unordered_map<uchar, size_t> chr2drop;

    vector<Token> tokens;
    vector<TokenSpan> token2clean;
};

#endif  // CC_FRONTEND_FRONTEND_RESULT_H_
