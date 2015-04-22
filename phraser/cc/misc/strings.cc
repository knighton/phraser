#include "strings.h"

namespace strings {

void Split(const string& s, char c, vector<string>* v) {
    v->clear();
    size_t prev_c = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == c) {
            v->emplace_back(s.substr(prev_c, i - prev_c));
            prev_c = i + 1;
        }
    }
    v->emplace_back(s.substr(prev_c, s.size() - prev_c));
}

#define INDEX_DNE ~0ul

void SplitByWhitespace(const string& s, vector<string>* v) {
    v->clear();

    size_t last_nonspace_i = INDEX_DNE;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (isspace(c)) {
            if (last_nonspace_i != INDEX_DNE) {
                v->emplace_back(s.substr(last_nonspace_i, i - last_nonspace_i));
                last_nonspace_i = INDEX_DNE;
            }
        } else {
            if (last_nonspace_i == INDEX_DNE) {
                last_nonspace_i = i;
            }
        }
    }
    if (last_nonspace_i != INDEX_DNE) {
        v->emplace_back(s.substr(last_nonspace_i));
    }
}

#undef INDEX_DNE

}  // namespace string
