#ifndef CC_EXPRESSION_EXPRESSION_EVALUATOR_H_
#define CC_EXPRESSION_EXPRESSION_EVALUATOR_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/expression/expression.h"

using std::string;
using std::unordered_map;
using std::vector;

// Base class for each class that evaluates a single type of Expression.
//
// (Expressions have a type field like "verb", "tag", etc.)
class ExpressionTypeEvaluator {
  public:
    virtual ~ExpressionTypeEvaluator();

    // Is the expression possible?
    //
    // Reasons for false that are common to all Expression type Evaluators:
    // * Its type does not match my type
    // * It has a filter dimension I have never heard of
    //
    // Reasons for false that are per evaluator:
    // * Its args are invalid (override AreArgsPossible()).
    bool IsExpressionPossible(const Expression& expr) const;

  protected:
    // Check the expression's args.
    //
    // Called internally by IsExpressionPossible().
    virtual bool AreArgsPossible(const vector<string>& args) const = 0;

    string type_;
    unordered_map<string, unordered_set<string>> dimension2possible_values_;
};

#endif  // CC_EXPRESSION_EXPRESSION_EVALUATOR_H_
