#ifndef CC_EXPRESSION_EXPRESSION_EVALUATOR_H_
#define CC_EXPRESSION_EXPRESSION_EVALUATOR_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/expression/all_at_once_evaluator.h"
#include "cc/expression/dynamic_evaluator.h"
#include "cc/expression/expression_evaluator.h"
#include "cc/expression/precomputable_evaluator.h"
#include "cc/misc/json.h"

using std::string;
using std::unordered_map;
using std::vector;

// We map input tokens to lists of integers (TokenGroupIDs).
//
// TokenGroupIDs are not necessarily consecutive.  TokenGroupIDs refer to
// either (a) an Expression that matched the token, or (b) a verbatim token
// present in the lookup index that matched.
//
// If even: divide by two, and that is the index in the expression_ vector.
//
// If odd: divide by two, and that is the index in the tokens_ vector.
typedef uint32_t TokenGroupID;

template <typename ExprEval>
struct TypeHandler {
    ExprEval* evaluator;
    vector<TokenGroupID> token_group_ids;
};

// A dictionary of Expressions and tokens.
class ExprEvalVocabulary {
  public:
    // Dump to JSON.
    json::Object* ToJSON() const;

    // Returns whether it already existed.
    bool IdentifyToken(const string& token, size_t* index);

    const string& GetToken(size_t index) const;

    // Returns whether it already existed.
    bool IdentifyExpression(
        const string& canon_expr_str, const Expression& expr, size_t* index);

    const Expression& GetExpression(size_t index) const;

    void Clear();

  private:
    vector<string> tokens_;
    unordered_map<string, size_t> token2index_;

    vector<Expression> expressions_;
    unordered_map<string, size_t> exprstr2index_;
};

class ExpressionEvaluator {
  public:
    ~ExpressionEvaluator();

    // Nuke everything.  You can't just do wipe out Evaluators or just the
    // vocabulary due to the precomputing.
    void Clear();

    // Set the evaluators to use.  Calls Clear() first.
    //
    // Takes ownership of pointers.
    bool InitWithEvaluators(
        const unordered_map<string, PrecomputableEvaluator*>& type2precompoute,
        const unordered_map<string, DynamicEvaluator*>& type2dynamic,
        const unordered_map<string, AllAtOnceEvaluator<string>*>&
            type2allatonce);

    // Dump to JSON.
    json::Object* ToJSON() const;

    // Add the token to my internal dictionary.
    //
    // Returns false on error.
    bool AddToken(const string& token, TokenGroupID* group_id);

    // Returns whether it succeeded.
    bool ParseExpression(const string& raw_expr_str, Expression* expr,
                         string* error) const;

    // Parse an Expression and add it to my internal dictionary (if it's
    // precomputable, add all the tokens it results in, else just keep it
    // around to call during EvaluateTokens() dynamically).
    //
    // Returns false on error (such as invalid Expression string).
    bool AddExpression(const string& raw_expr_str, TokenGroupID* group_id,
                       string* error);

    // Map token -> list of TokenGroupIDs.
    //
    // Returned lists will usually be empty (depending on how promiscuous your
    // config is).  If called before Init(), the caller will get empty lists.
    //
    // Returns false on error.
    bool EvaluateTokens(const vector<string>& tokens,
                        vector<vector<TokenGroupID>>* group_id_lists,
                        string* error) const;

    // TokenGroupID -> human-friendly string.
    //
    // Returns false on invalid TokenGroupID.
    bool GetPrettyTokenGroup(TokenGroupID group_id, string* pretty) const;

  private:

    // Find the ExpressionTypeEvaluator subclass for a Expression type (checking
    // the three *_type2handler_ lookup tables).
    ExpressionTypeEvaluator* FindExpressionEvaluator(
            const string& expr_type) const;

    // Precompute what we can.  Add an entry to the appropriate handler.
    bool IndexAnExpression(const Expression& expr, TokenGroupID group_id,
                           string* error);

    // Expression type -> ExpressionEvaluator and indexes of all the Expressions
    // of that type.  Split by which type of ExpressionEvaluator it is because
    // they behave differently.
    unordered_map<string, TypeHandler<PrecomputableEvaluator>>
        precomputable_type2handler_;
    unordered_map<string, TypeHandler<DynamicEvaluator>> dynamic_type2handler_;
    unordered_map<string, TypeHandler<AllAtOnceEvaluator<string>>>
        all_at_once_type2handler_;

    // All the Expressions and tokens that this object knows.
    ExprEvalVocabulary vocab_;

    // Token -> list of TokenGroupIDs.
    //
    // We get them from:
    // * AddToken()
    // * AddExpression() of a PrecomputableExpressions
    //
    // Get the rest dynamically during EvaluateTokens()..
    unordered_map<string, vector<TokenGroupID>> token2precomputed_;
};

#endif  // CC_EXPRESSION_EXPRESSION_EVALUATOR_H_
