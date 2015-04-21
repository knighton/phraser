#ifndef CC_BASE_TOKEN_CATEGORIZATION_DYNAMIC_EVALUATOR_H_
#define CC_BASE_TOKEN_CATEGORIZATION_DYNAMIC_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/base/token_categorization/expression.h"

class DynamicEvaluator : Expression {
  public:
    virtual ~DynamicEvaluator() = 0;

    // Quick check to rule out a match by an expression of my type.
    virtual bool MightMatch(const string& token) const = 0;

    // Does the expression accept the token?
    virtual bool IsMatch(const Expression& expr, const string& token) const = 0;
};

#endif  // CC_BASE_TOKEN_CATEGORIZATION_DYNAMIC_EVALUATOR_H_
