#include "pos_det_evaluator.h"

bool PosDetEvaluator::Init(PersonalManager* personal_mgr) {
    type_ = "posdet";
    dimension2possible_values_ = {
        {"number", {"sing", "plur"}},
        {"person", {"1st", "2nd", "3rd"}},
        {"gender", {"male", "female", "neuter"}},
    };
    personal_mgr_ = personal_mgr;
    return true;
}

bool PosDetEvaluator::GetExpressionMatches(
        const Expression& expr, vector<string>* tokens) const {
    return personal_mgr_->GetExpressionMatches(expr, tokens);
}

bool PosDetEvaluator::AreArgsPossible(const vector<string>& args) const {
    return args.empty();
}
