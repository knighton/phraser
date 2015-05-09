#ifndef CC_FRONTEND_BASE_TOKEN_NORM_REWRITE_IN_PLACE_STEP_H_
#define CC_FRONTEND_BASE_TOKEN_NORM_REWRITE_IN_PLACE_STEP_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class TokenNormRewriteInPlaceStep {
  public:
    virtual ~TokenNormRewriteInPlaceStep() {}

    virtual bool RewriteInPlace(vector<string>* inout, string* error) const = 0;
};

#endif  // CC_FRONTEND_BASE_TOKEN_NORM_REWRITE_IN_PLACE_STEP_H_
