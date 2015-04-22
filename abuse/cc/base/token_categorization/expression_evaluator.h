#ifndef CC_BASE_TOKEN_CATEGORIZATION_EXPRESSION_EVALUATOR_H_
#define CC_BASE_TOKEN_CATEGORIZATION_EXPRESSION_EVALUATOR_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/base/token_categorization/expression.h"

using std::string;
using std::unordered_map;
using std::vector;

class ExpressionEvaluator {
  public:
    virtual ~ExpressionEvaluator();

    // Is the expression possible?
    //
    // Reasons for false that are common to all Evaluators:
    // * Its type does not match my type
    // * It has a filter dimension I have never heard of
    //
    // Reasons for false that are per Evaluator:
    // * Its args are invalid (override AreArgsPossible()).
    bool IsExpressionPossible(const Expression& expr) const;

  protected:
    // Check the expression's args.
    //
    // Called internally by IsExpressionPossible().
    virtual bool AreArgsPossible(const vector<string>& args) const = 0;

    string type_;
    unordered_map<string, unordered_set<string>> dimension2values_;
};

#endif  // CC_BASE_TOKEN_CATEGORIZATION_EXPRESSION_EVALUATOR_H_
