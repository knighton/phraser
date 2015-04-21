#ifndef CC_BASE_TOKEN_CATEGORIZATION_TOKEN_CATEGORIZER_H_
#define CC_BASE_TOKEN_CATEGORIZATION_TOKEN_CATEGORIZER_H_

#include "cc/base/token_categorization/expression.h"
#include "cc/base/token_categorization/open_class_type_evaluator.h"
#include "cc/base/token_categorization/open_class_type_all_token_evaluator.h"
#include "cc/base/token_categorization/closed_class_type_evaluator.h"

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

struct OpenClassTypeInfo {
    OpenClassTypeEvaluator evaluator;
    vector<ExpressionID> expr_ids;
};

struct OpenClassTypeAllTokenInfo {
    OpenClassTypeAllTokenEvaluator evaluator;
    vector<ExpressionID> expr_ids;
};

class TokenCategorizer {
  public:
    // Construct indexes for translating tokens to CategoryIDs.
    //
    // Returns false on failure:
    // * Invalid Expression encountered
    bool Init(
        const unordered_map<string, ClosedClassTypeEvaluator*>& type2closed,
        const unordered_map<string, OpenClassTypeEvaluator*>& type2open,
        const unordered_map<
                string, OpenClassTypeAllTokenEvaluator*>& type2openvector,
        const vector<Expression>& expressions,
        const vector<string>& raw_tokens);

    // Token -> CategoryIDs, per token.
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

    // Type -> OpenClassTypeEvaluator.
    unordered_map<string, OpenClassTypeInfo> open_type2info_;

    // Type -> OpenClassTypeAllTokenEvaluator.
    unordered_map<string, OpenClassTypeAllTokenInfo> open_all_token_type2info_;
};

#endif // CC_BASE_TOKEN_CATEGORIZATION_TOKEN_CATEGORIZER_H_
