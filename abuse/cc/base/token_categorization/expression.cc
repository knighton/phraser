#include "expression.h"

#include <algorithm>

void Expression::Init(
        const string& type, const vector<string>& args,
        const unordered_map<string, vector<string>>& dimension2filters) {
    type_ = type;
    args_ = args;
    dimension2filters_ = dimension2filters;
}

void Expression::ToCanonicalString(string* s) const {
    s->clear();

    *s += type_;
    *s += ' ';

    for (auto& arg : args_) {
        *s += arg;
        *s += ' ';
    }

    vector<string> dims;
    for (auto& it : dimension2filters_) {
        dims.emplace_back(it.first);
    }
    sort(dims.begin(), dims.end());

    vector<string> v;
    for (auto& dim : dims) {
        v.clear();
        for (auto& filter : dimension2filters_.find(dim)->second) {
            v.emplace_back(filter);
        }
        sort(v.begin(), v.end());
        *s += dim;
        *s += '=';
        if (v.size()) {
            *s += v[0];
        }
        for (auto i = 1u; i < v.size(); ++i) {
            *s += ',';
            *s += v[i];
        }
    }
}
