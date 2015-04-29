#ifndef CC_TOKENIZATION_ASCII_TOKENIZER_H_
#define CC_TOKENIZATION_ASCII_TOKENIZER_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class LaposASCIITokenizer {
  public:
    void Tokenize(const string& s, vector<string>* tokens) const;
};

#endif  // CC_TOKENIZATION_ASCII_TOKENIZER_H_
