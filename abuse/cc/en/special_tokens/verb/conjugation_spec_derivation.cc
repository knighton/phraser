#include "conjugation_spec_derivation.h"

void ConjugationSpecDerivation::Init(
        const StringTransform& pres_part, const StringTransform& past_part,
        const vector<StringTransform>& presents,
        const vector<StringTransform>& pasts) {
    pres_part_ = pres_part;
    past_part_ = past_part;
    presents_ = presents;
    pasts_ = pasts;
}

void ConjugationSpecDerivation::Derive(
        const string& lemma, ConjugationSpec* spec) const {
    string pres_part;
    string past_part;
    vector<string> presents;
    presents.reserve(presents_.size());
    vector<string> pasts;
    pasts.reserve(pasts_.size());

    pres_part_.Apply(lemma, &pres_part);
    past_part_.Apply(lemma, &past_part);
    for (auto& t : presents_) {
        string s;
        t.Apply(lemma, &s);
        presents.emplace_back(s);
    }
    for (auto& t : pasts_) {
        string s;
        t.Apply(lemma, &s);
        pasts.emplace_back(s);
    }
    spec->Init(lemma, pres_part, past_part, presents, pasts);
}
