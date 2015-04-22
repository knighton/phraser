#include "verb_evaluator.h"

#include <string>
#include <unordered_set>
#include <vector>

#include "cc/expression/token_with_features.h"
#include "cc/english/verb/conjugation_spec.h"

using std::string;
using std::unordered_set;
using std::vector;

VerbEvaluator::VerbEvaluator() : conj_(NULL) {
}

VerbEvaluator::~VerbEvaluator() {
    if (conj_) {
        delete conj_;
        conj_ = NULL;
    }
}

void VerbEvaluator::Init(Conjugator* conj) {
    type_ = "verb";
    dimension2possible_values_.clear();
    conj_ = conj;
}

bool VerbEvaluator::GetExpressionMatches(
        const Expression& expr, vector<string>* tokens) const {
    tokens->clear();

    if (!conj_) {
        return false;
    }

    const string& lemma = expr.args()[0];
    ConjugationSpec spec;
    conj_->Conjugate(lemma, &spec);

    vector<TokenWithFeatures> tokens_with_features;
    if (!spec.EachTokenWithFeatures(&tokens_with_features)) {
        return false;
    }

    unordered_set<string> tokens_set;
    for (auto& t : tokens_with_features) {
        if (expr.AcceptsFeatures(t.dimension2value)) {
            tokens_set.insert(t.token);
        }
    }

    for (auto& token : tokens_set) {
        tokens->emplace_back(token);
    }
    return true;
}

bool VerbEvaluator::AreArgsPossible(const vector<string>& args) const {
    return args.size() == 1;
}
