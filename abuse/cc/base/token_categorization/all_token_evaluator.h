#ifndef CC_BASE_TOKEN_CATEGORIZATION_ALL_TOKEN_EVALUATOR_H_
#define CC_BASE_TOKEN_CATEGORIZATION_ALL_TOKEN_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/base/token_categorization/evaluator.h"
#include "cc/base/token_categorization/expression.h"

using std::string;
using std::vector;

template <typename TokenDescription>
class AllTokenEvaluator : public Evaluator {
  public:
    // Tokens -> list of corresponding opaque objects (eg. tags, maps of
    // features, etc.).
    virtual void AnalyzeTokens(
        const vector<string>& tokens,
        vector<TokenDescription>* descs) const = 0;

    // Does the expression accept the token given the additional info?
    virtual bool IsMatch(
        const Expression& expr, const string& token,
        const TokenDescription& desc) const = 0;
};

#endif  // CC_BASE_TOKEN_CATEGORIZATION_ALL_TOKEN_EVALUATOR_H_
