#ifndef CC_EXPRESSION_EXPRESSION_TYPE_EVALUATOR_H_
#define CC_EXPRESSION_EXPRESSION_TYPE_EVALUATOR_H_

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

    const string& type() const { return type_; }

    // Is the expression possible?
    //
    // Reasons for false that are common to all Expression type Evaluators:
    // * Its type does not match my type
    // * It has a filter dimension I have never heard of
    //
    // Reasons for false that are per evaluator:
    // * Its args are invalid (override AreArgsPossible()).
    bool IsExpressionPossible(const Expression& expr) const;

    // Given a list of dimension values, arrange them according to which
    // dimension they belong to (which are not given).
    //
    // Returns false if unkonwn value.
    bool OrganizeExpressionDimensionValues(
        const vector<string>& filters,
        unordered_map<string, unordered_set<string>>* values) const;

  protected:
    // Check the expression's args.
    //
    // Called internally by IsExpressionPossible().
    virtual bool AreArgsPossible(const vector<string>& args) const = 0;

    string type_;
    unordered_map<string, unordered_set<string>> dimension2possible_values_;
    unordered_map<string, string> value2dimension_;
};

#endif  // CC_EXPRESSION_EXPRESSION_TYPE_EVALUATOR_H_
