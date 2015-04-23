#include "expression_evaluator.h"

#include <algorithm>
#include <unordered_set>

using std::unordered_set;

ExpressionEvaluator::ExpressionEvaluator() {
}

ExpressionEvaluator::~ExpressionEvaluator() {
    Clear();
}

static TokenGroupID TokenGroupIDFromExpressionIndex(size_t index) {
    return static_cast<TokenGroupID>(index * 2);
}

static TokenGroupID TokenGroupIDFromRawTokenIndex(size_t index) {
    return static_cast<TokenGroupID>(index * 2 + 1);
}

IndexExpressionResult ExpressionEvaluator::IndexPrecomputableExpression(
        const unordered_map<string, PrecomputableEvaluator*>& type2precompute,
        const Expression& expr, TokenGroupID group_id) {
    auto it = type2precompute.find(expr.type());
    if (it == type2precompute.end()) {
        return IER_NOT_MY_TYPE;
    }

    auto* evaluator = it->second;
    vector<string> tokens;
    if (!evaluator->GetExpressionMatches(expr, &tokens)) {
        return IER_INVALID;
    }

    sort(tokens.begin(), tokens.end());
    for (auto& token : tokens) {
        token2groupids_[token].push_back(group_id);
    }

    precompute_type2handler_[expr.type()].group_ids.emplace_back(group_id);
    return IER_SUCCESS;
}

IndexExpressionResult ExpressionEvaluator::IndexDynamicExpression(
        const Expression& expr, GroupID group_id) {
    auto it = dynamic_type2handler_.find(expr.type());
    if (it == dynamic_type2handler_.end()) {
        return IER_NOT_MY_TYPE;
    }

    auto& handler = it->second;
    if (!handler.evaluator->IsExpressionPossible(expr)) {
        return IER_INVALID;
    }

    handler.token_group_ids.push_back(group_id);
    return IER_SUCCESS;
}

IndexExpressionResult ExpressionEvaluator::IndexAllTokenExpression(
        const Expression& expr, TokenGroupID group_id) {
    auto it = all_input_type2handler_.find(expr.type());
    if (it == all_input_type2handler_.end()) {
        return IER_NOT_MY_TYPE;
    }

    auto& handler = it->second;
    if (!handler.evaluator->IsExpressionPossible(expr)) {
        return IER_INVALID;
    }

    handler.token_group_ids.push_back(group_id);
    return IER_SUCCESS;
}

void ExpressionEvaluator::Clear() {
    expressions_.clear();
    raw_tokens_.clear();
    token2groupids_.clear();

    for (auto& it : precompute_type2handler_) {
        auto& handler = it.second;
        if (handler.evaluator) {
            delete handler.evaluator;
        }
    }

    for (auto& it : dynamic_type2handler_) {
        auto& handler = it.second;
        if (handler.evaluator) {
            delete handler.evaluator;
        }
    }

    for (auto& it : all_input_type2handler_) {
        auto& handler = it.second;
        if (handler.evaluator) {
            delete handler.evaluator;
        }
    }

    precompute_type2handler_.clear();
    dynamic_type2handler_.clear();
    all_input_type2handler_.clear();
}

bool ExpressionEvaluator::InitWithEvaluatorsAndData(
        const unordered_map<string, PrecomputableEvaluator*>& type2precompute,
        const unordered_map<string, DynamicEvaluator*>& type2dynamic,
        const unordered_map<string, AllInputEvaluator<string>*>& type2all_input,
        const vector<Expression>& all_unique_expressions,
        const vector<string>& all_unique_raw_tokens) {
    Clear();

    // Verify uniqueness of the raw tokens list.
    unordered_set<string> raw_tokens_set;
    for (auto& raw_token : all_unique_raw_tokens) {
        if (raw_tokens_set.find(raw_token) != raw_tokens_set.end()) {
            return false;
        }
        raw_tokens_set.insert(raw_token);
    }
    raw_tokens_ = all_unique_raw_tokens;

    // Save the precompute expressions for future reference (but they are not
    // used in LookUpTokens() because they are precomputed).
    for (auto& it : type2precompute) {
        precompute_type2handler_[it.first].evaluator = it.second;
    }

    // Save the dynamic (open class) type evaluators, because we'll need them in
    // LookUpTokens().
    for (auto& it : type2dynamic) {
        dynamic_type2handler_[it.first].evaluator = it.second;
    }
    for (auto& it : type2all_input) {
        all_input_type2handler_[it.first].evaluator = it.second;
    }

    // For each expression,
    unordered_set<string> expr_strings_set;  // Set of Expression canonical str.
    for (auto i = 0u; i < all_unique_expressions.size(); ++i) {
        auto& expr = expressions[i];

        // Bail if we have seen the expression before.
        string expr_str;
        expr.ToCanonicalString(&expr_str);
        if (expr_strings_set.find(expr_str) != expr_strings_set.end()) {
            return false;
        }

        // Note the new expression.
        expressions_.emplace_back(expr);
        TokenGroupID group_id =
            TokenGroupIDFromExpressionIndex(exprstr2groupid.size());
        expr_strings_set.insert(expr_str);

        // If it was recognized as a precompute type, get the list of tokens
        // that the Expression yields when evaluated and index them.
        //
        // Expressions can only be one type, so if we got a match, we're done.
        auto r = IndexPrecomputableExpression(type2precompute, expr, group_id);
        if (r == IER_INVALID) {
            return false;
        } else if (r == IER_SUCCESS) {
            continue;
        }

        // For dynamic types, we can only verify that the generating expression
        // is valid (has dimensions we know about and is the right type).

        // Single-token dynamic expressions.
        r = IndexDynamicExpression(expr, group_id);
        if (r == IER_INVALID) {
            return false;
        } else if (r == IER_SUCCESS) {
            continue;
        }

        // Dymamic expressions that require all the tokens at once.
        r = IndexAllTokenExpression(expr, group_id);
        if (r == IER_INVALID) {
            return false;
        } else if (r == IER_SUCCESS) {
            continue;
        }

        // Unknown type.
        return false;
    }

    return true;
}

bool ExpressionEvaluator::LookUpTokens(
        const vector<string>& tokens,
        vector<vector<TokenGroupID>>* group_id_lists) const {
    group_id_lists->clear();
    group_id_lists->resize(tokens.size());

    // For each input token,
    for (auto i = 0u; i < tokens.size(); ++i) {
        auto& token = tokens[i];
        auto& group_ids = (*group_id_lists)[i];

        // First, check our precomputed string -> TokenGroupID lookup table.
        {
            auto it = token2groupids_.find(token);
            if (it != token2groupids_.end()) {
                group_ids = it->second;
            }
        }

        // Then, run the open class types (like regex) against the token.
        for (auto& it : dynamic_type2handler_) {
            auto& handler = it.second;

            // Cheap check first to rule things out.
            if (!handler.evaluator->MightMatch(token)) {
                continue;
            }

            // Then, get features from the token.
            unordered_map<string, string> features;
            handler.evaluator->FeaturesFromToken(token, &features);

            // If it might match, check each expression for a match.
            for (auto& group_id : handler.token_group_ids) {
                auto& expr = expressions_[group_id];
                if (expr.AcceptsFeatures(features)) {
                    group_ids.emplace_back(group_id);
                }
            }
        }
    }

    // Finally, run the expression evaluators that require all tokens at once
    // (like tagging).
    for (auto& it : all_input_type2handler_) {
        auto& handler = it.second;

        // No Expressions that require that type?  Skip it.
        if (!handler.token_group_ids.size()) {
            continue;
        }

        // Classify each token.
        vector<string> analyses;
        if (!handler.evaluator->AnalyzeTokens(tokens, &analyses)) {
            return false;
        }

        // Evaluate the Expressions against that.  Append the IDs of the ones
        // that match.
        for (auto i = 0u; i < analyses.size(); ++i) {
            auto& extra = analyses[i];
            for (auto& group_id : handler.token_group_ids) {
                auto& expr = expressions_[group_id];
                if (handler.evaluator->IsMatch(expr, tokens[i], extra)) {
                    (*group_id_lists)[i].emplace_back(group_id);
                }
            }
        }
    }

    return true;
}

void ExpressionEvaluator::GetPrettyTokenGroup(
        TokenGroupID group_id, string* pretty) const {
    if (group_id % 2) {
        *pretty = raw_tokens_[group_id / 2];
    } else {
        auto& expr = expressions_[group_id / 2];
        expr.ToCanonicalString(pretty);
    }
}
