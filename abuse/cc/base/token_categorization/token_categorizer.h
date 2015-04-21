#ifndef CC_BASE_TOKEN_CATEGORIZATION_TOKEN_CATEGORIZER_H_
#define CC_BASE_TOKEN_CATEGORIZATION_TOKEN_CATEGORIZER_H_

#include "cc/base/token_categorization/all_token_evaluator.h"
#include "cc/base/token_categorization/expression.h"
#include "cc/base/token_categorization/one_token_evaluator.h"
#include "cc/base/token_categorization/precompute_evaluator.h"

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

struct PrecomputeOneTokenStuff {
    PrecomputeEvaluator* evaluator;
    vector<ExpressionID> expr_ids;
};

struct DynamicOneTokenStuff {
    OneTokenEvaluator* evaluator;
    vector<ExpressionID> expr_ids;
};

struct DynamicAllTokenStuff {
    AllTokenEvaluator<string>* evaluator;
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
    bool Init(
        const unordered_map<string, PrecomputeEvaluator*>& type2precompute,
        const unordered_map<string, OneTokenEvaluator*>& type2dynamic,
        const unordered_map<string,
            AllTokenEvaluator<string>*>& type2all_token_dymamic,
        const vector<Expression>& expressions,
        const vector<string>& raw_tokens);

    // Nuke everything.  Deletes all objects (ie, the Evaluators).
    void Clear();

    // Token -> CategoryIDs, per token.
    //
    // If called before Init(), the caller will get empty vectors.
    void CategorizeTokens(const vector<string>& tokens,
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

    IndexExpressionResult IndexPrecomputeExpression(
        const unordered_map<string, PrecomputeEvaluator*>& type2precompute,
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

    // Type -> PrecomputeEvaluator and Expression list.
    //
    // Not used during CategorizeTokens(), as the results of the Expressions
    // are precomputed.
    unordered_map<string, PrecomputeOneTokenStuff> precompute_type2stuff_;

    // Type -> OneTokenEvaluator and Expression list.
    unordered_map<string, DynamicOneTokenStuff> dynamic_type2stuff_;

    // Type -> AllTokenEvaluator and Expression list.
    unordered_map<string, DynamicAllTokenStuff> all_token_dynamic_type2stuff_;
};

#endif // CC_BASE_TOKEN_CATEGORIZATION_TOKEN_CATEGORIZER_H_
