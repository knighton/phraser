#include "expression.h"

#include <algorithm>

void Expression::Init(
        const string& type, const vector<string>& args,
        const unordered_map<string, unordered_set<string>>& dimension2values) {
    type_ = type;
    args_ = args;
    dimension2values_ = dimension2values;
}

bool Expression::AcceptsFeatures(
        const unordered_map<string, string>& dim2value) const {
    for (auto& it : dim2value) {
        auto& dim_we_have = it.first;

        auto jt = dimension2values_.find(dim_we_have);
        if (jt == dimension2values_.end()) {
            return false;
        }

        auto& value_we_have = it.second;
        auto& values_we_need = jt->second;
        if (values_we_need.find(value_we_have) == values_we_need.end()) {
            return false;
        }
    }
    return true;
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
    for (auto& it : dimension2values_) {
        for (auto& jt : it.second) {
            all_values.emplace_back(jt);
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
