#ifndef CC_BASE_TOKEN_CATEGORIZATION_EVALUATOR_H_
#define CC_BASE_TOKEN_CATEGORIZATION_EVALUATOR_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "cc/base/token_categorization/expression.h"

using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_set;

// Expression Evaluator.  Tells if an Expression matches a token.
//
// Hierarchy of base classes:
//
// Evaluator (abstract)
// * OneTokenEvaluator (abstract)
//   * DynamicEvaluator
//   * PrecomputeEvaluator
// * AllTokenEvaluator
class Evaluator {
  public:
    virtual ~Evaluator();

    // Is the expression possible?
    //
    // Reasons for false that are common to all Evaluators:
    // * Its type does not match my type
    // * It has a filter dimension I have never heard of
    //
    // Reasons for false that are per Evaluator:
    // * Its args are invalid (override AreArgsPossible()).
    bool IsExpressionPossible(const Expression& expr) const;

  private:
    // Check the expression's args.
    //
    // Called internally by IsExpressionPossible().
    virtual bool AreArgsPossible(const vector<string>& args) const = 0;

    string type_;
    unordered_map<string, unordered_set<string>> dimension2values_;
};

#endif  // CC_BASE_TOKEN_CATEGORIZATION_EVALUATOR_H_
