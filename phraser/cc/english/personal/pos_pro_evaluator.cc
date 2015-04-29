#include "pos_pro_evaluator.h"

bool PosProEvaluator::Init(PersonalManager* personal_mgr) {
    type_ = "pospro";
    dimension2possible_values_ = {
        {"ppcase", {"subj", "obj", "refl"}},
        {"number", {"sing", "plur"}},
        {"person", {"1st", "2nd", "3rd"}},
        {"gender", {"male", "female", "neuter"}},
    };
    personal_mgr_ = personal_mgr;
    return true;
}

bool PosProEvaluator::GetExpressionMatches(
        const Expression& expr, vector<string>* tokens) const {
    return personal_mgr_->GetExpressionMatches(expr, tokens);
}

bool PosProEvaluator::AreArgsPossible(
        const vector<string>& args, string* error) const {
    if (args.size()) {
        *error = "[PosProEvaluator] Must have 0 args.";
        return false;
    }

    return true;
}
