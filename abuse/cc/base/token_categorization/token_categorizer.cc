#include "token_categorizer.h"

bool TokenCategorizer::Init(
        const unordered_map<string, ClosedClassEvaluator>& type2closed,
        const unordered_map<string, OpenClassEvaluator>& type2open,
        const vector<Expression>& expressions,
        const vector<string>& raw_tokens) {
    expression_.clear();
    raw_tokens_.clear();

    // Save the open-class type evaluators, because we'll need them in
    // CategorizeTokens().
    for (auto& it : type2open) {
        open_type2info_[it->first] = it->second;
    }

    // For each expression,
    for (auto i = 0u; i < expressions.size(); ++i) {
        auto& expr = expressions[i];

        // Skip if we have seen the expression before.
        string expr_str;
        expr.ToCanonicalString(&expr_str);
        auto& it = exprstr2catid_.find(expr_str);
        if (it != exprstr2catid_.end()) {
            continue;
        }

        // Note the new expression.
        expressions_.emplace_back(expr);
        ExpressionID expr_id = exprstr2catid_.size();
        exprstr2catid_[*it] = expr_id;

        // If it was recognized as a closed-class type, precompute the
        // expression by generating its tokens and indexing them.
        auto& it = type2closed.find(expr.type());
        if (it != type2closed.end()) {
            auto& evaluator = *it;
            vector<string> tokens;
            sort(tokens.begin(), tokens.end());
            for (auto& token : tokens) {
                token2catids_[token].push_back(expr_id);
            }
            continue;
        }

        // For open-class types, we can only verify that the generating
        // expression is valid.
        //
        // Since it wasn't a closed class type, it must be an open class type.
        auto& jt = open_type2info_.find(jt);
        if (jt == open_type2info_.end()) {
            return false;
        }

        if (!jt->evaluator.IsValidExpression(expr)) {
            return false;
        }

        jt->expr_ids.push_back(expr_id);
    }

    // Extract the unique raw tokens.
    unordered_set<string> raw_token_set;
    for (auto& raw_token : raw_tokens) {
        raw_token_set.insert(raw_token);
    }
    raw_tokens_.reserve(raw_token_set.size());
    for (auto& it = raw_token_set.begin(); it != raw_token_set.end(); ++it) {
        raw_tokens_.emplace_back(*it);
    }

    return true;
}

void TokenCategorizer::CategorizeTokens(
        const vector<string>& tokens,
        vector<vector<CategoryID>>* cat_id_lists) const {
    cat_id_lists->clear();
    cat_id_lists->resize(tokens.size());

    // For each input token,
    for (auto& i = 0u; i < tokens.size(); ++i) {
        auto& token = tokens[i];
        auto& cat_ids = (*cat_id_lists)[i];

        // First, check our precomputed string -> CategoryID lookup table.
        auto& it = token2catids_.find(token);
        if (it != token2catids_.end()) {
            cat_ids = it->second;
            continue;
        }

        // If that found nothing, run the open class types (like regex) against
        // the token.
        for (auto& it = open_type2info_.begin(); it != open_type2info_.end();
             ++it) {
            if (!it->second.evaluator.IsTypeMatch(token)) {
                continue;
            }
            for (auto& expr_id : it->second.expr_ids) {
                auto& expr = expressions_[expr_id];
                if (it->second.evaluator.IsExpressionMatch(expr, token)) {
                    cat_ids.emplace_back(expr_id);
                }
            }
            if (cat_ids.size()) {
                break;
            }
        }
    }

    // Finally, run the expression evaluators that require all tokens at once
    // (like tagging).
    for (auto& it = open_all_token_type2info_.begin(); it !=
         open_all_token_type2info_.end(); ++it) {
        // No Expressions that require that type?  Skip it.
        if (!it->second.expr_ids.size()) {
            continue;
        }

        // Classify each token.
        vector<string> clf_results;
        it->second.evaluator.ClassifyTokens(tokens, &clf_results);

        // Evaluate the Expressions against that.  Append the IDs of the ones
        // that match.
        for (auto i = 0u; i < clf_results.size(); ++i) {
            auto& clf_result = clf_results[i];
            for (auto& expr_id : it->second.expr_ids) {
                auto& expr = expressions_[expr_id];
                if (expr.IsMatch(clf_result)) {
                    (*cat_id_lists)[i].emplace_back(expr_id);
                }
            }
        }
    }
}

bool TokenCategorizer::GetExpressionID(
        const string& expr_canonical_string, ExpressionID* expr_id) const {
    string s;
    auto& it = exprstr2catid_.find(expr_canonical_string);
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
