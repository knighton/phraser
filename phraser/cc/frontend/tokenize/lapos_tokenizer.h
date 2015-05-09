#ifndef CC_FRONTEND_TOKENIZE_LAPOS_TOKENIZER_H_
#define CC_FRONTEND_TOKENIZE_LAPOS_TOKENIZER_H_

#include <string>
#include <vector>

#include "cc/frontend/base/token_span.h"

using std::string;
using std::vector;

class LaposTokenizer {
  public:
    bool Tokenize(const string& s, vector<string>* tokens,
                  vector<TokenSpan>* out2in) const;
};

#endif  // CC_FRONTEND_TOKENIZE_LAPOS_TOKENIZER_H_
