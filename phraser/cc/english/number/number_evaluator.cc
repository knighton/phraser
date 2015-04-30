#include "number_evaluator.h"

using boost::cmatch;

bool NumberEvaluator::Init(string* error) {
    type_ = "number";
    dimension2possible_values_ = {
        {"class",    {"int", "float"}},
        {"polarity", {"nonneg", "neg"}},
    };
    might_match_re_ = regex("[-+]?\\d+(\\.\\d+)?");

    return PostInit(error);
}

bool NumberEvaluator::MightMatch(const string& token) const {
    cmatch match;
    return regex_match(token.data(), match, might_match_re_);
}

void NumberEvaluator::FeaturesFromToken(
        const string& token, unordered_map<string, string>* dim2value) const {
    bool has_period = false;
    bool has_dash = token.size() && token[0] == '-';
    for (auto& c : token) {
        if (c == '.') {
            has_period = true;
            break;
        }
    }

    dim2value->clear();
    (*dim2value)["class"] = has_period ? "float" : "int";
    (*dim2value)["polarity"] = has_dash ? "neg" : "nonneg";
}

bool NumberEvaluator::AreArgsPossible(const vector<string>& args, string* error) const {
    if (args.size() != 0) {
        *error = "[NumberEvaluator] Must have 0 args.";
        return false;
    }

    return true;
}
