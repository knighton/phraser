#include "expression_evaluator.h"

#include <algorithm>

#include "cc/misc/strings.h"

bool ExprEvalVocabulary::IdentifyToken(const string& token, size_t* index) {
    auto it = token2index_.find(token);
    if (it != token2index_.end()) {
        *index = it->second;
        return true;
    }

    *index = tokens_.size();
    token2index_[token] = *index;
    tokens_.emplace_back(token);
    return false;
}

const string& ExprEvalVocabulary::GetToken(size_t index) const {
    return tokens_[index];
}

bool ExprEvalVocabulary::IdentifyExpression(
        const string& canon_expr_str, const Expression& expr, size_t* index) {
    auto it = exprstr2index_.find(canon_expr_str);
    if (it != exprstr2index_.end()) {
        *index = it->second;
        return true;
    }

    *index = expressions_.size();
    exprstr2index_[canon_expr_str] = *index;
    expressions_.emplace_back(expr);
    return false;
}

const Expression& ExprEvalVocabulary::GetExpression(size_t index) const {
    return expressions_[index];
}

json::Object* ExprEvalVocabulary::ToJSON() const {
    vector<string> expr_ss;
    expr_ss.reserve(expressions_.size());
    for (auto& expr : expressions_) {
        string s;
        expr.ToCanonicalString(&s);
        expr_ss.emplace_back(s);
    }

    return new json::Object({
        {"tokens", new json::Object(tokens_)},
        {"expressions", new json::Object(expr_ss)},
    });
}

void ExprEvalVocabulary::Clear() {
    tokens_.clear();
    token2index_.clear();
    expressions_.clear();
    exprstr2index_.clear();
}

static TokenGroupID TokenGroupIDFromTokenIndex(size_t index) {
    return static_cast<TokenGroupID>(index * 2 + 1);
}

static size_t TokenIndexFromTokenGroupID(TokenGroupID group_id) {
    return static_cast<size_t>(group_id / 2);
}

static TokenGroupID TokenGroupIDFromExpressionIndex(size_t index) {
    return static_cast<TokenGroupID>(index * 2);
}

static size_t ExpressionIndexFromTokenGroupID(TokenGroupID group_id) {
    return static_cast<size_t>(group_id / 2);
}

void ExpressionEvaluator::Clear() {
    for (auto& it : precomputable_type2handler_) {
        auto& e = it.second.evaluator;
        if (e) {
            delete e;
            e = NULL;
        }
    }
    for (auto& it : dynamic_type2handler_) {
        auto& e = it.second.evaluator;
        if (e) {
            delete e;
            e = NULL;
        }
    }
    for (auto& it : all_at_once_type2handler_) {
        auto& e = it.second.evaluator;
        if (e) {
            delete e;
            e = NULL;
        }
    }

    vocab_.Clear();

    token2precomputed_.clear();
}

ExpressionEvaluator::~ExpressionEvaluator() {
    Clear();
}

bool ExpressionEvaluator::InitWithEvaluators(
        const unordered_map<string, PrecomputableEvaluator*>& type2precompute,
        const unordered_map<string, DynamicEvaluator*>& type2dynamic,
        const unordered_map<string, AllAtOnceEvaluator<string>*>&
            type2allatonce) {
    Clear();

    for (auto& it : type2precompute) {
        auto& eval = it.second;
        if (!eval) {
            return false;
        }

        precomputable_type2handler_[it.first].evaluator = eval;
    }

    for (auto& it : type2dynamic) {
        auto& eval = it.second;
        if (!eval) {
            return false;
        }

        dynamic_type2handler_[it.first].evaluator = eval;
    }

    for (auto& it : type2allatonce) {
        auto& eval = it.second;
        if (!eval) {
            return false;
        }

        all_at_once_type2handler_[it.first].evaluator = eval;
    }

    return true;
}

json::Object* ExpressionEvaluator::ToJSON() const {
    map<string, json::Object*> type2precomputable;
    for (auto& it : precomputable_type2handler_) {
        type2precomputable[it.first] = it.second.evaluator->ToJSON();
    }

    map<string, json::Object*> type2dynamic;
    for (auto& it : dynamic_type2handler_) {
        type2dynamic[it.first] = it.second.evaluator->ToJSON();
    }

    map<string, json::Object*> type2allatonce;
    for (auto& it : all_at_once_type2handler_) {
        type2allatonce[it.first] = it.second.evaluator->ToJSON();
    }

    map<string, json::Object*> token2precomputed;
    for (auto& it : token2precomputed_) {
        token2precomputed[it.first] = new json::Object(it.second);
    }

    return new json::Object({
        {"precomputable_evaluators", new json::Object(type2precomputable)},
        {"dynamic_evaluators", new json::Object(type2dynamic)},
        {"all_at_once_evaluators", new json::Object(type2allatonce)},
        {"vocabulary", vocab_.ToJSON()},
        {"token2precompted", new json::Object(token2precomputed)},
    });
}

bool ExpressionEvaluator::AddToken(
        const string& token, TokenGroupID* group_id) {
    // Get/create the token's TokenGroupID.
    size_t index;
    bool found = vocab_.IdentifyToken(token, &index);
    *group_id = TokenGroupIDFromTokenIndex(index);

    // If we've never seen it before, add it to the precomputed table.
    if (!found) {
        token2precomputed_[token].emplace_back(*group_id);
    }

    return true;
}

bool ExpressionEvaluator::IndexAnExpression(
        const Expression& expr, TokenGroupID group_id, string* error) {
    // Get the handler for the Expression.  There can be only one, since each
    // Expression Evaluator implements a different Expression type.
    //
    // We split the mappings by Evaluator type because they have to be treated
    // differently in EvaluteTokens().

    // PrecomputableEvaluator: precompute.
    auto it = precomputable_type2handler_.find(expr.type());
    if (it != precomputable_type2handler_.end()) {
        auto& handler = it->second;
        vector<string> tokens;
        if (!handler.evaluator->GetExpressionMatches(expr, &tokens)) {
            *error = "[ExpressionEvaluator] PrecomputableEvaluator "
                     "GetExpressionMatches() failed.";
            return false;
        }

        for (auto& token : tokens) {
            token2precomputed_[token].emplace_back(group_id);
        }

        handler.token_group_ids.emplace_back(group_id);
        return true;
    }

    // DynamicEvaluator: check validity.
    auto jt = dynamic_type2handler_.find(expr.type());
    if (jt != dynamic_type2handler_.end()) {
        auto& handler = jt->second;
        if (!handler.evaluator->IsExpressionPossible(expr, error)) {
            return false;
        }

        handler.token_group_ids.emplace_back(group_id);
        return true;
    }

    // AllAtOnceEvaluator: check validity.
    auto kt = all_at_once_type2handler_.find(expr.type());
    if (kt != all_at_once_type2handler_.end()) {
        auto& handler = kt->second;
        if (!handler.evaluator->IsExpressionPossible(expr, error)) {
            return false;
        }

        handler.token_group_ids.emplace_back(group_id);
        return true;
    }

    return false;
}

static bool ParseRawExpression(
        const string& raw_expr_str, string* type, vector<string>* args,
        vector<string>* filters, string* error) {
    type->clear();
    args->clear();
    filters->clear();

    vector<string> v;
    strings::SplitByWhitespace(raw_expr_str, &v);

    if (v.empty()) {
        return false;
    }

    if (v[0].empty()) {
        return false;
    }

    if (v[0][0] == '+') {
        return false;
    }

    *type = v[0];

    auto i = 1u;
    for (; i < v.size(); ++i) {
        if (v[i].empty()) {
            *error = "[ExpressionEvaluator] Empty arg.";
            return false;
        }
        if (v[i][0] == '+') {
            break;
        }
        args->emplace_back(v[i]);
    }

    for (; i < v.size(); ++i) {
        if (v[i].empty()) {
            *error = "[ExpressionEvaluator] Empty filter.";
            return false;
        }
        if (v[i][0] != '+') {
            *error = "[ExpressionEvaluator] Filters must start with '+'.  "
                     "Input: [" + v[i] + "].";
            return false;
        }
        filters->emplace_back(v[i].substr(1));
    }

    return true;
}

ExpressionTypeEvaluator* ExpressionEvaluator::FindExpressionEvaluator(
        const string& expr_type) const {
    // TODO: replace this with a type -> handler lookup table.
    auto it = precomputable_type2handler_.find(expr_type);
    if (it != precomputable_type2handler_.end()) {
        return it->second.evaluator;
    }

    auto jt = dynamic_type2handler_.find(expr_type);
    if (jt != dynamic_type2handler_.end()) {
        return jt->second.evaluator;
    }

    auto kt = all_at_once_type2handler_.find(expr_type);
    if (kt != all_at_once_type2handler_.end()) {
        return kt->second.evaluator;
    }

    return NULL;
}

bool ExpressionEvaluator::ParseExpression(
        const string& raw_expr_str, Expression* expr, string* error) const {
    string type;
    vector<string> args;
    vector<string> filters;
    if (!ParseRawExpression(raw_expr_str, &type, &args, &filters, error)) {
        return false;
    }

    ExpressionTypeEvaluator* evaluator = FindExpressionEvaluator(type);
    if (!evaluator) {
        *error = "[ExpressionEvaluator] No expression evaluator exists for "
                 "this type of expression: [" + type + "]";
        return false;
    }

    unordered_map<string, unordered_set<string>> dim2values;
    if (!evaluator->OrganizeExpressionDimensionValues(
            filters, &dim2values, error)) {
        return false;
    }

    expr->Init(type, args, dim2values);
    return true;
}

bool ExpressionEvaluator::AddExpression(
        const string& raw_expr_str, TokenGroupID* group_id, string* error) {
    // Try to parse an Expression out of the string.
    Expression expr;
    if (!ParseExpression(raw_expr_str, &expr, error)) {
        return false;
    }

    // Dump the Expression to a new string in canonical form.
    string canon_expr_str;
    expr.ToCanonicalString(&canon_expr_str);

    // Get/create the Expression's TokenGroupID.
    size_t index;
    bool found = vocab_.IdentifyExpression(canon_expr_str, expr, &index);
    *group_id = TokenGroupIDFromExpressionIndex(index);

    // If it's already been processed, we're done.
    if (found) {
        return true;
    }

    return IndexAnExpression(expr, *group_id, error);
}

bool ExpressionEvaluator::EvaluateTokens(
        const vector<string>& tokens,
        vector<vector<TokenGroupID>>* group_id_lists, string* error) const {
    group_id_lists->clear();
    group_id_lists->resize(tokens.size());

    // For each input token,
    for (auto i = 0u; i < tokens.size(); ++i) {
        auto& token = tokens[i];
        auto& group_ids = (*group_id_lists)[i];

        // First, check our precomputed string -> TokenGroupID lookup table.  It
        // will have the results from all the precomputable Expressions.
        {
            auto it = token2precomputed_.find(token);
            if (it != token2precomputed_.end()) {
                group_ids = it->second;
            }
        }

        // Then, run the open class one token types (like regex) against it.
        for (auto& it : dynamic_type2handler_) {
            auto& handler = it.second;

            // If we have no Expressions to match of that type, skip it.
            if (handler.token_group_ids.empty()) {
                continue;
            }

            // Do a cheap check first to rule things out.
            if (!handler.evaluator->MightMatch(token)) {
                continue;
            }

            // If we couldn't rule it out, get features from the token.
            unordered_map<string, string> features;
            handler.evaluator->FeaturesFromToken(token, &features);

            // Check each expression for a match.
            for (auto& group_id : handler.token_group_ids) {
                size_t index = ExpressionIndexFromTokenGroupID(group_id);
                auto& expr = vocab_.GetExpression(index);
                if (expr.AcceptsFeatures(features)) {
                    group_ids.emplace_back(group_id);
                }
            }
        }
    }

    // For each all-at-once Expression evaluator,
    for (auto& it : all_at_once_type2handler_) {
        auto& handler = it.second;

        // If we have no Expressions to match of that type, skip it.
        if (handler.token_group_ids.empty()) {
            continue;
        }

        // Classify each token.
        vector<string> tags;  // Call it "tags", but it could be any kind of
                              // annotation.
        if (!handler.evaluator->AnalyzeTokens(tokens, &tags, error)) {
            return false;
        }

        // Evaluate the Expressions against that result.  Append the
        // TokenGroupIDs of the ones that match.
        for (auto i = 0u; i < tags.size(); ++i) {
            auto& tag = tags[i];
            for (auto& group_id : handler.token_group_ids) {
                size_t index = ExpressionIndexFromTokenGroupID(group_id);
                auto& expr = vocab_.GetExpression(index);
                if (handler.evaluator->IsMatch(expr, tokens[i], tag)) {
                    (*group_id_lists)[i].emplace_back(group_id);
                }
            }
        }
    }

    return true;
}

bool ExpressionEvaluator::GetPrettyTokenGroup(
        TokenGroupID group_id, string* pretty) const {
    if (group_id % 2) {
        size_t index = TokenIndexFromTokenGroupID(group_id);
        *pretty = vocab_.GetToken(index);
    } else {
        size_t index = ExpressionIndexFromTokenGroupID(group_id);
        auto& expr = vocab_.GetExpression(index);
        expr.ToCanonicalString(pretty);
    }
    return true;
}
