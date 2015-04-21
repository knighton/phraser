#ifndef LAPOS_TOKENIZE_H_
#define LAPOS_TOKENIZE_H_

#include <string>
#include <vector>

#include "common.h"

using std::string;
using std::vector;

void tokenize(const string& s1, vector<string>& lt);

void tokenize(const string& s, vector<Token>& vt,
              const bool use_upenn_tokenizer);

#endif  // LAPOS_TOKENIZE_H_
