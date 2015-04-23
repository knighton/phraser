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

// We map tokens to lists of integers (TokenGroupIDs).  The integers are:
// (a) IDs of unique Expressions present in the input
// (b) IDs of unique raw tokens present in the input (starting after the
//     Expression IDs).
//
// If the Expression was not in the input, it will not be recognized.  In
// LookUpTokens(), words that have not been seen before are not assigned a token
// ID, as they will not be matching anything by token (although they may match
// an Expression).
//
//     [0                                        UINT32_MAX]
//     [           TokenGroupIDs             ] [unused-----]
//     [Expression IDs] [Raw Lookup Token IDs] [unused-----]
typedef uint32_t TokenGroupID;

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

    // Construct indexes for translating tokens to TokenGroupIDs.
    //
    // Takes ownership of the pointers.
    //
    // Returns false on failure:
    // * Invalid Expression encountered
    bool InitWithEvaluatorsAndData(
        const unordered_map<string, PrecomputableEvaluator*>& type2precompute,
        const unordered_map<string, DynamicEvaluator*>& type2dynamic,
        const unordered_map<string,
            AllInputEvaluator<string>*>& type2all_token_dymamic,
        const vector<Expression>& expressions,
        const vector<string>& raw_tokens);

    // Nuke everything.  Deletes all objects (ie, the Evaluators).
    void Clear();

    // Token -> TokenGroupIDs (Expression IDs + RawLookupTokenIDs), per token.
    //
    // If called before Init(), the caller will get empty vectors.
    //
    // Returns false on internal error.
    bool LookUpTokens(
        const vector<string>& tokens,
        vector<vector<TokenGroupID>>* group_id_lists) const;

    // Canonical string form of an Expression -> TokenGroupID.
    //
    // Returns false on unknown Expression.
    bool GetExpressionID(const string& expr_canonical_string,
                         TokenGroupID* group_id) const;

    // TokenGroupID -> human-friendly string.
    //
    // TokenGroupID must be valid.
    void GetPrettyTokenGroup(TokenGroupID group_id, string* pretty) const;

  private:
    // Init() helpers.

    IndexExpressionResult IndexPrecomputableExpression(
        const unordered_map<string, PrecomputableEvaluator*>& type2precompute,
        const Expression& expr, TokenGroupID group_id);

    IndexExpressionResult IndexDynamicExpression(
        const Expression& expr, TokenGroupID group_id);

    IndexExpressionResult IndexAllInputExpression(
        const Expression& expr, TokenGroupID group_id);

    // All expressions in the index.
    //
    // Expression IDs correspond to these.
    vector<Expression> expressions_;

    // Each unique raw token in the Init() input.
    //
    // TokenGroupIDs offset by expressions_.size() corresdpong to these.
    vector<string> raw_tokens_;

    // Expression canonical string -> TokenGroupID.
    unordered_map<string, TokenGroupID> exprstr2groupid_;

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
