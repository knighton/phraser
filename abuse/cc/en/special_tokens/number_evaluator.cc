#include "number_evaluator.h"

using boost::cmatch;

void NumberEvaluator::Init() {
    type_ = "number";
    dimension2values_ = {
        {
            "class",
            {
                "int",
                "float",
            },
        },
        {
            "polarity",
            {
                "nonneg",
                "neg",
            }
        },
    };

    might_match_re_ = regex("(-|+)?[0-9]+(\\.[0-9]+)?");
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

bool NumberEvaluator::AreArgsPossible(const vector<string>& args) const {
    return args.size() == 0;
}
