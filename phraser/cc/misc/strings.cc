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

void Trim(string* s) {
    if (s->empty()) {
        return;
    }

    auto begin = ~0u;
    for (auto i = 0u; i < s->size(); ++i) {
        auto& c = (*s)[i];
        if (!isspace(c)) {
            begin = i;
            break;
        }
    }

    if (begin == ~0u) {
        s->clear();
        return;
    }

    auto end = ~0u;
    for (auto i = s->size() - 1; i != ~0u; --i) {
        auto& c = (*s)[i];
        if (isspace(c)) {
            end = i;
            break;
        }
    }

    *s = s->substr(begin, end - begin);
}

}  // namespace string
