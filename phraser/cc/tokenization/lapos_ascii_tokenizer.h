#ifndef CC_TOKENIZATION_LAPOS_ASCII_TOKENIZER_H_
#define CC_TOKENIZATION_LAPOS_ASCII_TOKENIZER_H_

#include <string>
#include <vector>

#include "cc/tokenization/span.h"

using std::string;
using std::vector;

class LaposASCIITokenizer {
  public:
    bool Tokenize(const string& s, vector<string>* tokens,
                  vector<Span>* out2in) const;
};

#endif  // CC_TOKENIZATION_LAPOS_ASCII_TOKENIZER_H_
