#ifndef CC_FRONTEND_BASE_TOKEN_NORM_REWRITE_IN_PLACE_STEP_H_
#define CC_FRONTEND_BASE_TOKEN_NORM_REWRITE_IN_PLACE_STEP_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class TokenRewriteInPlaceStep {
  public:
    virtual ~TokenRewriteInPlaceStep() {}

    virtual bool RewriteInPlace(vector<string>* inout, string* error) const = 0;
};

#endif  // CC_FRONTEND_BASE_TOKEN_NORM_REWRITE_IN_PLACE_STEP_H_
