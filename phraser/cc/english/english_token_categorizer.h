#ifndef CC_ENGLISH_ENGLISH_TOKEN_CATEGORIZER_H_
#define CC_ENGLISH_ENGLISH_TOKEN_CATEGORIZER_H_

#include "cc/expression/expression.h"
#include "cc/expression/token_categorizer.h"

#include <string>

using std::string;

class EnglishTokenCategorizer : public TokenCategorizer {
  public:
    // Returns false on error.
    bool InitWithData(const string& lapos_model_f,
                      const vector<Expression>& expressions,
                      const vector<string>& raw_tokens);
};

#endif  // CC_ENGLISH_ENGLISH_TOKEN_CATEGORIZER_H_
