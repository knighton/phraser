#ifndef CC_EXPRESSION_PRECOMPUTABLE_EVALUATOR_H_
#define CC_EXPRESSION_PRECOMPUTABLE_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/expression/expression_evaluator.h"
#include "cc/expression/expression.h"

using std::string;
using std::vector;

class PrecomputableEvaluator : public ExpressionEvaluator {
  public:
    // Expression -> list of tokens that it matches.
    //
    // Returns false on invalid Expression.
    virtual bool GetExpressionMatches(
        const Expression& expr, vector<string>* tokens) const = 0;
};

#endif  // CC_EXPRESSION_PRECOMPUTABLE_EVALUATOR_H_
