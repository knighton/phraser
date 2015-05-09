#include "americanizer_step.h"

#include "cc/base/strings.h"
#include "cc/frontend/americanize/americanizer_data.h"

static bool MakeToken2Token(const string& s2z_s, const string& pairs_s,
                            unordered_map<string, string>* token2token,
                            string* error) {
    token2token->clear();

    vector<string> v;

    strings::SplitByWhitespace(s2z_s, &v);
    for (auto& s : v) {
        auto z = s;
        for (auto i = 0u; i < z.size(); ++i) {
            if (z[i] == 's') {
                z[i] = 'z';
            }
        }

        auto it = token2token->find(s);
        if (it != token2token->end()) {
            *error = "[AmericanizerStep] Duplicate key encountered: [" + s +
                     "].";
            return false;
        }

        (*token2token)[s] = z;
    }

    strings::SplitByWhitespace(pairs_s, &v);
    if (v.size() % 2) {
        return false;
    }

    for (auto i = 0u; i < v.size(); i += 2) {
        auto& from_s = v[i];
        auto& to_s = v[i + 1];

        auto it = token2token->find(from_s);
        if (it != token2token->end()) {
            *error = "[AmericanizerStep] Duplicate key encountered: [" + from_s
                     + "].";
            return false;
        }

        (*token2token)[from_s] = to_s;
    }

    return true;
}

bool AmericanizerStep::Init(string* error) {
    if (!MakeToken2Token(
            americanizer_data::S2Z_LIST, americanizer_data::TOKEN_PAIRS,
            &s2s_, error)) {
        return false;
    }

    return true;
}

bool AmericanizerStep::RewriteInPlace(
        vector<string>* inout, string* error) const {
    for (auto& s : *inout) {
        auto it = s2s_.find(s);
        if (it != s2s_.end()) {
            s = it->second;
        }
    }
    return true;
}
