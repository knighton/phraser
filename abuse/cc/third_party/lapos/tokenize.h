#ifndef CC_THIRD_PARTY_LAPOS_TOKENIZE_H_
#define CC_THIRD_PARTY_LAPOS_TOKENIZE_H_

#include <string>
#include <vector>

#include "cc/third_party/lapos/common.h"

using std::string;
using std::vector;

void tokenize(const string& s1, vector<string>& lt);

void tokenize(const string& s, vector<Token>& vt,
              const bool use_upenn_tokenizer);

#endif  // CC_THIRD_PARTY_LAPOS_TOKENIZE_H_
