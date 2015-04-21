#ifndef CC_BASE_TOKEN_CATEGORIZATION_PRECOMPUTE_EVALUATOR_H_
#define CC_BASE_TOKEN_CATEGORIZATION_PRECOMPUTE_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/base/token_categorization/expression.h"

using std::string;
using std::vector;

class PrecomputeEvaluator {
  public:
    virtual ~PrecomputeEvaluator() = 0;

    // Expression -> list of tokens that it matches.
    //
    // Returns false on invalid Expression.
    virtual bool GetExpressionMatches(
        const Expression& expr, vector<string>* tokens) const = 0;
};

#endif  // CC_BASE_TOKEN_CATEGORIZATION_PRECOMPUTE_EVALUATOR_H_
