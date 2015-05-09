#include "lapos_tagger.h"

#include <map>

using std::map;

#include "cc/third_party/lapos/common.h"
#include "cc/third_party/lapos/crf.h"
#include "cc/third_party/lapos/crfpos.h"
#include "cc/third_party/lapos/tokenize.h"

LaposTagger::LaposTagger() : is_ok_(false) {
}

LaposTagger::~LaposTagger() {
}

bool LaposTagger::Init() {
    if (!crfm_.init_default()) {
        return false;
    }

    is_ok_ = true;
    return true;
}

bool LaposTagger::Tag(const vector<string>& ss,
                      vector<string>* tags, string* error) {
    if (!is_ok_) {
        *error = "[LaposTagger] Tagger was not intialized.";
        return false;
    }

    // TODO: construct lapos::Tokens ourselves.
    string s;
    if (ss.size()) {
        s += ss[0];
    }
    for (auto i = 1u; i < ss.size(); ++i) {
        s += " ";
        s += ss[i];
    }

    vector<lapos::Token> tokens;
    lapos::tokenize(s.c_str(), tokens, false);

    vector<map<string, double>> tag_p0;
    lapos::crf_decode_lookahead(tokens, crfm_, tag_p0);

    tags->resize(ss.size());
    for (auto i = 0u; i < tag_p0.size(); ++i) {
        map<double, string> f2s;
        for (auto& it : tag_p0[i]) {
            f2s[it.second] = it.first;
        }
        auto it = f2s.rbegin();
        if (it == f2s.rend()) {
            *error = "[LaposTagger] No tag probabilites for token.";
            return false;
        }
        (*tags)[i] = it->second;
    }

    return true;
}
