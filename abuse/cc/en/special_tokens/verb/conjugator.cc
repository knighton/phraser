#include "conjugator.h"

void Conjugator::Init(
        const unordered_map<string, ConjugationSpec>& lemma2irregular,
        const ConjugationSpecDerivation& regular_deriv) {
    lemma2irregular_ = lemma2irregular;
    regular_deriv_ = regular_deriv;
}

void Conjugator::Conjugate(const string& lemma, ConjugationSpec* spec) const {
    auto it = lemma2irregular_.find(lemma);
    if (it != lemma2irregular_.end()) {
        *spec = it->second;
        return;
    }

    regular_deriv_.Derive(lemma, spec);
}
