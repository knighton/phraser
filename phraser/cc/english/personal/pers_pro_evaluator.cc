#include "pers_pro_evaluator.h"

bool PersProEvaluator::Init(PersonalManager* personal_mgr, string* error) {
    type_ = "perspro";
    dimension2possible_values_ = {
        {"ppcase", {"subj", "obj", "refl"}},
        {"number", {"sing", "plur"}},
        {"person", {"1st", "2nd", "3rd"}},
        {"gender", {"male", "female", "neuter"}},
    };
    personal_mgr_ = personal_mgr;
    return PostInit(error);
}

bool PersProEvaluator::GetExpressionMatches(
        const Expression& expr, vector<string>* tokens) const {
    return personal_mgr_->GetExpressionMatches(expr, tokens);
}

bool PersProEvaluator::AreArgsPossible(
        const vector<string>& args, string* error) const {
    if (args.size()) {
        *error = "[PersProEvaluator] Must have 0 args.";
        return false;
    }

    return true;
}
