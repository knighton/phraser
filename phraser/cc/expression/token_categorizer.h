#ifndef CC_EXPRESSION_TOKEN_CATEGORIZER_H_
#define CC_EXPRESSION_TOKEN_CATEGORIZER_H_

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

// We map tokens to lists of integers (CategoryIDs).
//
//     0                                UINT32_MAX
//     [        CategoryIDs        ] [unused-----]
//     [ExpressionIDs] [RawTokenIDs] [unused-----]
//            |              +--unique 'raw_tokens' passed to Init()
//            +-----------------unique 'expressions' pased to Init()
typedef uint32_t CategoryID;
typedef CategoryID ExpressionID;

struct PrecomputableStuff {
    PrecomputableEvaluator* evaluator;
    vector<ExpressionID> expr_ids;
};

struct DynamicStuff {
    DynamicEvaluator* evaluator;
    vector<ExpressionID> expr_ids;
};

struct AllInputStuff {
    AllInputEvaluator<string>* evaluator;
    vector<ExpressionID> expr_ids;
};

enum IndexExpressionResult {
    IER_NOT_MY_TYPE,
    IER_INVALID,
    IER_SUCCESS,
};

// Holds knowledge of how to detemrine which Expressions match a token.
//
// Converts tokens to lists of CategoryIDs (which are the ExpressionIDs + the
// raw token IDs).
class TokenCategorizer {
  public:
    TokenCategorizer();
    ~TokenCategorizer();

    // Construct indexes for translating tokens to CategoryIDs.
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

    // Token -> CategoryIDs, per token.
    //
    // If called before Init(), the caller will get empty vectors.
    //
    // Returns false on internal error.
    bool CategorizeTokens(const vector<string>& tokens,
                          vector<vector<CategoryID>>* cat_id_lists) const;

    // Canonical string form of an Expression -> ExpressionID.
    //
    // Returns false on unknown Expression.
    bool GetExpressionID(const string& expr_canonical_string,
                         ExpressionID* expr_id) const;

    // CategoryID -> human-friendly string.
    //
    // CategoryID must be valid.
    void GetPrettyCategory(CategoryID cat_id, string* pretty) const;

  private:
    // Init() helpers.

    IndexExpressionResult IndexPrecomputableExpression(
        const unordered_map<string, PrecomputableEvaluator*>& type2precompute,
        const Expression& expr, ExpressionID expr_id);

    IndexExpressionResult IndexDynamicExpression(
        const Expression& expr, ExpressionID expr_id);

    IndexExpressionResult IndexAllTokenExpression(
        const Expression& expr, ExpressionID expr_id);

    // All expressions in the index.
    //
    // Expression IDs correspond to these.
    vector<Expression> expressions_;

    // Each unique raw token in the Init() input.
    //
    // CategoryIDs offset by expressions_.size() corresdpong to these.
    vector<string> raw_tokens_;

    // Expression canonical string -> Category ID.
    unordered_map<string, CategoryID> exprstr2catid_;

    // Token -> precomputed Category IDs.
    unordered_map<string, vector<CategoryID>> token2catids_;

    // Type -> PrecomputableEvaluator and Expression list.
    //
    // Not used during CategorizeTokens(), as the results of the Expressions
    // are precomputed.
    unordered_map<string, PrecomputableStuff> precompute_type2stuff_;

    // Type -> DynamicEvaluator and Expression list.
    unordered_map<string, DynamicStuff> dynamic_type2stuff_;

    // Type -> AllInputEvaluator and Expression list.
    unordered_map<string, AllInputStuff> all_input_type2stuff_;
};

#endif // CC_EXPRESSION_TOKEN_CATEGORIZER_H_
