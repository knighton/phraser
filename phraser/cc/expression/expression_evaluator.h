#ifndef CC_EXPRESSION_EXPRESSION_EVALUATOR_H_
#define CC_EXPRESSION_EXPRESSION_EVALUATOR_H_

#include "cc/expression/all_input_evaluator.h"
#include "cc/expression/dynamic_evaluator.h"
#include "cc/expression/expression.h"
#include "cc/expression/precomputable_evaluator.h"

#include <string>
#include <vector>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_map;

// We map input tokens to lists of integers (TokenGroupIDs).
//
// TokenGroupIDs refer to either (a) an Expression that matched the token, or
// (b) a raw token present in the lookup index that matched.
//
// If even: divide by two, and that is the index in the expressions_ vector.
//
// If odd: divide by two, and that is the index in the raw_tokens_ vector.
//
// If the Expression was not in the input, it will not be recognized.  In
// LookUpTokens(), words that have not been seen before are not assigned a token
// ID, as they will not be matching anything by token (although they may match
// an Expression).
typedef int32_t TokenGroupID;

struct PrecomputableHandler {
    PrecomputableEvaluator* evaluator;
    vector<TokenGroupID> token_group_ids;
};

struct DynamicHandler {
    DynamicEvaluator* evaluator;
    vector<TokenGroupID> token_group_ids;
};

struct AllInputHandler {
    AllInputEvaluator<string>* evaluator;
    vector<TokenGroupID> token_group_ids;
};

enum IndexExpressionResult {
    IER_NOT_MY_TYPE,
    IER_INVALID,
    IER_SUCCESS,
};

// Holds knowledge of how to detemrine which Expressions match a token.
//
// Converts tokens to lists of TokenGroupIDs (which are the Expression IDs + the
// raw token IDs which are each groups of size one).
class ExpressionEvaluator {
  public:
    ExpressionEvaluator();
    ~ExpressionEvaluator();

    // Construct indexes for translating tokens to TokenGroupIDs.  The input
    // 'expressions' and 'raw_tokens' must be unique.
    //
    // Takes ownership of the pointers.
    //
    // Returns false on failure:
    // * Invalid Expression encountered
    // * Non-unique Expression list
    // * Non-unique raw token list
    bool InitWithEvaluatorsAndData(
        const unordered_map<string, PrecomputableEvaluator*>& type2precompute,
        const unordered_map<string, DynamicEvaluator*>& type2dynamic,
        const unordered_map<string,
            AllInputEvaluator<string>*>& type2all_token_dymamic,
        const vector<Expression>& all_unique_expressions,
        const vector<string>& all_unique_raw_tokens);

    // Nuke everything.  Deletes all objects (ie, the Evaluators).
    void Clear();

    // Token -> list of TokenGroupIDs per token.
    //
    // Returned lists will usually be empty (depending on how promiscuous your
    // config is).  If called before Init(), the caller will get empty lists.
    //
    // Returns false on internal error.
    bool Evaluate(
        const vector<string>& tokens,
        vector<vector<TokenGroupID>>* group_id_lists) const;

    // TokenGroupID -> human-friendly string.
    //
    // Returns false on invalid TokenGroupID.
    bool GetPrettyTokenGroup(TokenGroupID group_id, string* pretty) const;

  private:
    // Init() helpers.

    IndexExpressionResult IndexPrecomputableExpression(
        const unordered_map<string, PrecomputableEvaluator*>& type2precompute,
        const Expression& expr, TokenGroupID group_id);

    IndexExpressionResult IndexDynamicExpression(
        const Expression& expr, TokenGroupID group_id);

    IndexExpressionResult IndexAllInputExpression(
        const Expression& expr, TokenGroupID group_id);

    // All expressions in the index built in Init().
    //
    // Index * 2 = TokenGroupID of Expression.
    vector<Expression> expressions_;

    // Each unique raw token in the index built in Init().
    //
    // Index * 2 + 1 = TokenGroupID of raw token.
    vector<string> raw_tokens_;

    // Token -> precomputed TokenGroupIDs.
    unordered_map<string, vector<TokenGroupID>> token2groupids_;

    // Type -> PrecomputableEvaluator and Expression indexes.
    //
    // Not used during LookUpTokens(), as the results of the Expressions
    // are precomputed.
    unordered_map<string, PrecomputableHandler> precompute_type2stuff_;

    // Type -> DynamicEvaluator and Expression indexes.
    unordered_map<string, DynamicHandler> dynamic_type2stuff_;

    // Type -> AllInputEvaluator and Expression indexes.
    unordered_map<string, AllInputHandler> all_input_type2stuff_;
};

#endif // CC_EXPRESSION_EXPRESSION_EVALUATOR_H_
