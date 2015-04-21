#ifndef CC_BASE_TOKEN_CATEGORIZATION_ONE_TOKEN_EVALUATOR_H_
#define CC_BASE_TOKEN_CATEGORIZATION_ONE_TOKEN_EVALUATOR_H_

#include <string>
#include <unordered_map>

#include "cc/base/token_categorization/evaluator.h"
#include "cc/base/token_categorization/expression.h"

using std::string;
using std::unordered_map;

class OneTokenEvaluator : public Evaluator {
  public:
    // Quick check to rule out a match by an expression of my type.
    virtual bool MightMatch(const string& token) const = 0;

    // Extract token features for checking for Expression matches.
    virtual void FeaturesFromToken(
        const string& token,
        unordered_map<string, string>* dimension2value) const = 0;
};

#endif  // CC_BASE_TOKEN_CATEGORIZATION_ONE_TOKEN_EVALUATOR_H_
