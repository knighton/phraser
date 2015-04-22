#ifndef CC_EXPRESSION_DYNAMIC_EVALUATOR_H_
#define CC_EXPRESSION_DYNAMIC_EVALUATOR_H_

#include <string>
#include <unordered_map>

#include "cc/expression/expression_evaluator.h"
#include "cc/expression/expression.h"

using std::string;
using std::unordered_map;

class DynamicEvaluator : public ExpressionEvaluator {
  public:
    // Quick check to rule out a match by an expression of my type.
    virtual bool MightMatch(const string& token) const = 0;

    // Extract token features for checking for Expression matches.
    virtual void FeaturesFromToken(
        const string& token,
        unordered_map<string, string>* dimension2value) const = 0;
};

#endif  // CC_EXPRESSION_DYNAMIC_EVALUATOR_H_
