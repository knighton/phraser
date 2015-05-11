#include "unreductor.h"

#include "cc/frontend/reductions/reduction_data.h"

bool Unreductor::Init(
        const unordered_map<string, vector<Token>>& reduced2tokens,
        string* error) {
    reduced2tokens_ = reduced2tokens;
    return true;
}

bool Unreductor::InitDefault(string* error) {
    unordered_map<string, vector<Token>> reduced2tokens;
    for (auto& it : reduction_data::REDUCED2NORMAL) {
        auto& reduced = it.first;
        auto& to_lists = it.second;

        // Verify each normalized token list is the same length.
        if (to_lists.empty()) {
            return false;
        }
        auto required_size = to_lists[0].size();
        for (auto& to_list : to_lists) {
            if (to_list.size() != required_size) {
                *error = "[Unreductor] Token lists must be the same length.";
                return false;
            }
        }

        // For each token position, collect unique words and tags.
        //
        //     getcha -> get you/PRP, get your/PRP$
        auto num_tokens = required_size / 2;
        vector<Token> tokens;
        tokens.resize(num_tokens);
        for (auto i = 0u; i < num_tokens; ++i) {
            for (auto j = 0u; j < to_lists.size(); ++j) {
                auto& token = tokens[i];
                auto& value = to_lists[j][i * 2 + 0];
                auto& maybe_tag = to_lists[j][i * 2 + 1];
                auto found_value = false;
                for (auto& option : token.options) {
                    if (option.value == value) {
                        if (maybe_tag != option.tag) {
                            *error = "[Unreductor] Same token position, same "
                                     "string, different tag.";
                            return false;
                        }

                        found_value = true;
                        break;
                    }
                }

                if (found_value) {
                    continue;
                }

                token.options.resize(token.options.size() + 1);
                auto& option = token.options[token.options.size() - 1];
                option.value = value;
                option.tag = maybe_tag;
            }
        }

        reduced2tokens[reduced] = tokens;
    }

    return Init(reduced2tokens, error);
}

inline static Token& NewToken(vector<Token>* tokens) {
    tokens->resize(tokens->size() + 1);
    return (*tokens)[tokens->size() - 1];
}

void Unreductor::Rewrite(const vector<string>& in, vector<Token>* out,
                         vector<TokenIndex>* out2in) const {
    out->clear();
    out2in->clear();

    for (auto i = 0u; i < in.size(); ++i) {
        auto& s = in[i];

        // Handle not in the lookup table (normal case).
        auto it = reduced2tokens_.find(s);
        if (it == reduced2tokens_.end()) {
            auto& token = NewToken(out);
            token.options.resize(1);
            token.options[0].value = s;
            out2in->emplace_back(i);
            continue;
        }

        // Handle undoing the reduction.
        auto& tokens = it->second;
        out->insert(out->begin(), tokens.begin(), tokens.end());
        for (auto j = 0u; j < tokens.size(); ++j) {
            out2in->emplace_back(i);
        }
    }
}
