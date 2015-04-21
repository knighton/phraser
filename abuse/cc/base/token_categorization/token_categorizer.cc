#include "token_categorizer.h"

#include <algorithm>
#include <unordered_set>

using std::unordered_set;

TokenCategorizer::TokenCategorizer() {
}

TokenCategorizer::~TokenCategorizer() {
    Clear();
}

IndexExpressionResult TokenCategorizer::IndexPrecomputeExpression(
        const unordered_map<string, PrecomputeEvaluator*>& type2precompute,
        const Expression& expr, ExpressionID expr_id) {
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
        token2catids_[token].push_back(expr_id);
    }

    precompute_type2stuff_[expr.type()].expr_ids.emplace_back(expr_id);
    return IER_SUCCESS;
}

IndexExpressionResult TokenCategorizer::IndexDynamicExpression(
        const Expression& expr, ExpressionID expr_id) {
    auto it = dynamic_type2stuff_.find(expr.type());
    if (it == dynamic_type2stuff_.end()) {
        return IER_NOT_MY_TYPE;
    }

    auto& stuff = it->second;
    if (!stuff.evaluator->IsExpressionPossible(expr)) {
        return IER_INVALID;
    }

    stuff.expr_ids.push_back(expr_id);
    return IER_SUCCESS;
}

IndexExpressionResult TokenCategorizer::IndexAllTokenDynamicExpression(
        const Expression& expr, ExpressionID expr_id) {
    auto it = all_token_dynamic_type2stuff_.find(expr.type());
    if (it == all_token_dynamic_type2stuff_.end()) {
        return IER_NOT_MY_TYPE;
    }

    auto& stuff = it->second;
    if (!stuff.evaluator->IsExpressionPossible(expr)) {
        return IER_INVALID;
    }

    stuff.expr_ids.push_back(expr_id);
    return IER_SUCCESS;
}

void TokenCategorizer::Clear() {
    expressions_.clear();
    raw_tokens_.clear();
    exprstr2catid_.clear();
    token2catids_.clear();

    for (auto& it : precompute_type2stuff_) {
        auto& stuff = it.second;
        if (stuff.evaluator) {
            delete stuff.evaluator;
        }
    }

    for (auto& it : dynamic_type2stuff_) {
        auto& stuff = it.second;
        if (stuff.evaluator) {
            delete stuff.evaluator;
        }
    }

    for (auto& it : all_token_dynamic_type2stuff_) {
        auto& stuff = it.second;
        if (stuff.evaluator) {
            delete stuff.evaluator;
        }
    }

    precompute_type2stuff_.clear();
    dynamic_type2stuff_.clear();
    all_token_dynamic_type2stuff_.clear();
}

bool TokenCategorizer::Init(
        const unordered_map<string, PrecomputeEvaluator*>& type2precompute,
        const unordered_map<string, DynamicEvaluator*>& type2dynamic,
        const unordered_map<string, AllTokenDynamicEvaluator<string>*>&
            type2all_token_dynamic,
        const vector<Expression>& expressions,
        const vector<string>& raw_tokens) {
    Clear();

    // Save the precompute expressions for future reference (but they are not
    // used in CategorizeTokens() because they are precomputed).
    for (auto& it : type2precompute) {
        precompute_type2stuff_[it.first].evaluator = it.second;
    }

    // Save the dynamic (open class) type evaluators, because we'll need them in
    // CategorizeTokens().
    for (auto& it : type2dynamic) {
        dynamic_type2stuff_[it.first].evaluator = it.second;
    }
    for (auto& it : type2all_token_dynamic) {
        all_token_dynamic_type2stuff_[it.first].evaluator = it.second;
    }

    // For each expression,
    for (auto i = 0u; i < expressions.size(); ++i) {
        auto& expr = expressions[i];

        // Skip if we have seen the expression before.
        string expr_str;
        expr.ToCanonicalString(&expr_str);
        if (exprstr2catid_.find(expr_str) != exprstr2catid_.end()) {
            continue;
        }

        // Note the new expression.
        expressions_.emplace_back(expr);
        ExpressionID expr_id = static_cast<ExpressionID>(exprstr2catid_.size());
        exprstr2catid_[expr_str] = expr_id;

        // If it was recognized as a precompute type, get the list of tokens
        // that the Expression yields when evaluated and index them.
        //
        // Expressions can only be one type, so if we got a match, we're done.
        auto r = IndexPrecomputeExpression(type2precompute, expr, expr_id);
        if (r == IER_INVALID) {
            return false;
        } else if (r == IER_SUCCESS) {
            continue;
        }

        // For dynamic types, we can only verify that the generating expression
        // is valid (has dimensions we know about and is the right type).

        // Single-token dynamic expressions.
        r = IndexDynamicExpression(expr, expr_id);
        if (r == IER_INVALID) {
            return false;
        } else if (r == IER_SUCCESS) {
            continue;
        }

        // Dymamic expressions that require all the tokens at once.
        r = IndexAllTokenDynamicExpression(expr, expr_id);
        if (r == IER_INVALID) {
            return false;
        } else if (r == IER_SUCCESS) {
            continue;
        }

        // Unknown type.
        return false;
    }

    // Extract the unique raw tokens.
    unordered_set<string> raw_token_set;
    for (auto& raw_token : raw_tokens) {
        raw_token_set.insert(raw_token);
    }
    raw_tokens_.reserve(raw_token_set.size());
    for (auto& raw_token : raw_token_set) {
        raw_tokens_.emplace_back(raw_token);
    }

    return true;
}

void TokenCategorizer::CategorizeTokens(
        const vector<string>& tokens,
        vector<vector<CategoryID>>* cat_id_lists) const {
    cat_id_lists->clear();
    cat_id_lists->resize(tokens.size());

    // For each input token,
    for (auto i = 0u; i < tokens.size(); ++i) {
        auto& token = tokens[i];
        auto& cat_ids = (*cat_id_lists)[i];

        // First, check our precomputed string -> CategoryID lookup table.
        {
            auto it = token2catids_.find(token);
            if (it != token2catids_.end()) {
                cat_ids = it->second;
            }
        }

        // Then, run the open class types (like regex) against the token.
        for (auto& it : dynamic_type2stuff_) {
            auto& stuff = it.second;

            // Cheap check first to rule things out.
            if (!stuff.evaluator->MightMatch(token)) {
                continue;
            }

            // If it might match, check each expression for a match.
            for (auto& expr_id : stuff.expr_ids) {
                auto& expr = expressions_[expr_id];
                if (stuff.evaluator->IsMatch(expr, token)) {
                    cat_ids.emplace_back(expr_id);
                }
            }
        }
    }

    // Finally, run the expression evaluators that require all tokens at once
    // (like tagging).
    for (auto& it : all_token_dynamic_type2stuff_) {
        auto& stuff = it.second;

        // No Expressions that require that type?  Skip it.
        if (!stuff.expr_ids.size()) {
            continue;
        }

        // Classify each token.
        vector<string> descs;
        stuff.evaluator->DescribeTokens(tokens, &descs);

        // Evaluate the Expressions against that.  Append the IDs of the ones
        // that match.
        for (auto i = 0u; i < descs.size(); ++i) {
            auto& description = descs[i];
            for (auto& expr_id : stuff.expr_ids) {
                auto& expr = expressions_[expr_id];
                if (stuff.evaluator->IsMatch(expr, tokens[i], description)) {
                    (*cat_id_lists)[i].emplace_back(expr_id);
                }
            }
        }
    }
}

bool TokenCategorizer::GetExpressionID(
        const string& expr_canonical_string, ExpressionID* expr_id) const {
    string s;
    auto it = exprstr2catid_.find(expr_canonical_string);
    if (it == exprstr2catid_.end()) {
        return false;
    }

    *expr_id = it->second;
    return true;
}

void TokenCategorizer::GetPrettyCategory(
        CategoryID cat_id, string* pretty) const {
    if (cat_id < expressions_.size()) {
        auto& expr = expressions_[cat_id];
        expr.ToCanonicalString(pretty);
        return;
    }

    *pretty = raw_tokens_[cat_id - expressions_.size()];
}
