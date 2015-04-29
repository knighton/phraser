#include "expression_type_evaluator.h"

ExpressionTypeEvaluator::~ExpressionTypeEvaluator() {
}

bool ExpressionTypeEvaluator::IsExpressionPossible(
        const Expression& expr, string* error) const {
    if (expr.type() != type_) {
        *error = "[ExpressionTypeEvaluator] Type mismatch: [" + expr.type() +
                 "]";
        return false;
    }

    for (auto& it : expr.dimension2values()) {
        auto& expr_dimension = it.first;

        auto jt = dimension2possible_values_.find(expr_dimension);
        if (jt == dimension2possible_values_.end()) {
            *error = "[ExpressionTypeEvaluator] Unknown filter dimension: [" +
                     expr_dimension + "]";
            return false;
        }

        auto& expr_values_set = it.second;
        auto& evaluator_values_set = jt->second;
        for (auto& filter : expr_values_set) {
            if (evaluator_values_set.find(filter) ==
                    evaluator_values_set.end()) {
                *error = "[ExpressionTypeEvaluator] Unknown filter value: [" +
                         filter + "]";
                return false;
            }
        }
    }

    if (!AreArgsPossible(expr.args(), error)) {
        return false;
    }

    return true;
}

bool ExpressionTypeEvaluator::OrganizeExpressionDimensionValues(
        const vector<string>& values,
        unordered_map<string, unordered_set<string>>* dim2values,
        string* error) const {
    for (auto& value : values) {
        auto it = value2dimension_.find(value);
        if (it == value2dimension_.end()) {
            *error = "[ExpressionTypeEvaluator] Unknown filter value: [" +
                     value + "].";
            return false;
        }

        auto& dim = it->second;
        (*dim2values)[dim].insert(value);
    }

    return true;
}
