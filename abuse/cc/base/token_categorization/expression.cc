#include "expression.h"

#include <algorithm>

void Expression::Init(
        const string& type, const vector<string>& args,
        const unordered_map<string, vector<string>>& dimension2values) {
    type_ = type;
    args_ = args;
    dimension2values_ = dimension2values;
}

void Expression::ToCanonicalString(string* s) const {
    s->clear();

    *s += type_;
    *s += ' ';

    for (auto& arg : args_) {
        *s += arg;
        *s += ' ';
    }

    vector<string> all_values;
    for (auto& it : dimension2values) {
        for (auto& jt : it.second) {
            all_values.emplace_back(*jt);
        }
    }
    sort(all_values.begin(), all_values.end());

    if (all_values.size()) {
        *s += all_values[0];
    }
    for (auto i = 1u; i < all_values.size(); ++i) {
        *s += ' ';
        *s += all_values[i];
    }
}
