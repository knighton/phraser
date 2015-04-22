#include "lapos_token_preprocessor.h"

static bool Flip(const unordered_map<string, string>& in,
                 unordered_map<string, string>* out) {
    for (auto& it : in) {
        if (out->find(it.second) != out->end()) {
            return false;
        }
        (*out)[it.second] = it.first;
    }
    return true;
}

bool LaposTokenPreprocessor::Init() {
    conv_ = {
        {"(", "-LRB-"},
        {")", "-RRB-"},
        {"[", "-LSB-"},
        {"]", "-RSB-"},
        {"{", "-LCB-"},
        {"}", "-RCB-"},
    };

    if (!Flip(conv_, &unconv_)) {
        return false;
    }

    return true;
}

void LaposTokenPreprocessor::Preprocess(vector<string>* tokens) const {
    for (auto i = 0u; i < tokens->size(); ++i) {
        auto& token = (*tokens)[i];
        auto it = conv_.find(token);
        if (it == conv_.end()) {
            continue;
        }

        token = it->second;
    }
}
