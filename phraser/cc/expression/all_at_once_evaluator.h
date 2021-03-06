#ifndef CC_EXPRESSION_ALL_AT_ONCE_EVALUATOR_H_
#define CC_EXPRESSION_ALL_AT_ONCE_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/expression/expression_type_evaluator.h"
#include "cc/expression/expression.h"

using std::string;
using std::vector;

template <typename TokenDescription>
class AllAtOnceEvaluator : public ExpressionTypeEvaluator {
  public:
    // Tokens -> list of corresponding opaque objects (eg. tags, maps of
    // features, etc.) to be used by every Expression of this type.
    //
    // Returns false on error.
    virtual bool AnalyzeTokens(
        const vector<string>& tokens, vector<TokenDescription>* descs,
        string* error) const = 0;

    // Does the expression accept the token given the additional info?
    //
    // Assumes args have already been checked.
    virtual bool IsMatch(
        const Expression& expr, const string& token,
        const TokenDescription& desc) const = 0;
};

#endif  // CC_EXPRESSION_ALL_AT_ONCE_EVALUATOR_H_
