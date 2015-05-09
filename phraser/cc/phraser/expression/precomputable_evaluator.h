#ifndef CC_PHRASER_EXPRESSION_PRECOMPUTABLE_EVALUATOR_H_
#define CC_PHRASER_EXPRESSION_PRECOMPUTABLE_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/phraser/expression/expression_type_evaluator.h"
#include "cc/phraser/expression/expression.h"

using std::string;
using std::vector;

class PrecomputableEvaluator : public ExpressionTypeEvaluator {
  public:
    // Expression -> list of tokens that it matches.
    //
    // Returns false on invalid Expression.
    virtual bool GetExpressionMatches(
        const Expression& expr, vector<string>* tokens) const = 0;
};

#endif  // CC_PHRASER_EXPRESSION_PRECOMPUTABLE_EVALUATOR_H_
