#include "ptb_escaper_step.h"

bool PtbEscaperStep::Init(string* error) {
    // From https://www.cis.upenn.edu/~treebank/tokenizer.sed
    s2s_ = {
        {"(", "-LRB-"},
        {")", "-RRB-"},
        {"[", "-LSB-"},
        {"]", "-RSB-"},
        {"{", "-LCB-"},
        {"}", "-RCB-"},
    };
    return true;
}

bool PtbEscaperStep::RewriteInPlace(
        vector<string>* inout, string* error) const {
    for (auto& s : *inout) {
        auto it = s2s_.find(s);
        if (it != s2s_.end()) {
            s = it->second;
        }
    }
    return true;
}
