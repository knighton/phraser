#ifndef CC_FRONTEND_REDUCTIONS_UNREDUCTOR_H_
#define CC_FRONTEND_REDUCTIONS_UNREDUCTOR_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/frontend/base/token.h"
#include "cc/frontend/base/token_index.h"

using std::string;
using std::unordered_map;
using std::vector;

class Unreductor {
  public:
    // Returns false on fatal error.
    bool Init(const unordered_map<string, vector<Token>>& reduced2tokens,
              string* error);
    bool InitDefault(string* error);

    // Tokens containing reductions -> normalized tokens.
    void Rewrite(const vector<string>& in, vector<Token>* out,
                 vector<TokenIndex>* out2in) const;

  private:
    unordered_map<string, vector<Token>> reduced2tokens_;
};

#endif  // CC_FRONTEND_REDUCTIONS_UNREDUCTOR_H_
