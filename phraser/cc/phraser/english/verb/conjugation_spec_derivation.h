#ifndef CC_PHRASER_ENGLISH_VERB_CONJUGATION_SPEC_DERIVATION_H_
#define CC_PHRASER_ENGLISH_VERB_CONJUGATION_SPEC_DERIVATION_H_

#include <string>
#include <vector>

#include "cc/misc/string_transform.h"
#include "cc/english/verb/conjugation_spec.h"

using std::string;
using std::vector;

class ConjugationSpecDerivation {
  public:
    const StringTransform& pres_part() const { return pres_part_; }
    const StringTransform& past_part() const { return past_part_; }
    const vector<StringTransform>& presents() const { return presents_; }
    const vector<StringTransform>& pasts() const { return pasts_; }

    void InitFromTransforms(
        const StringTransform& pres_part, const StringTransform& past_part,
        const vector<StringTransform>& presents,
        const vector<StringTransform>& pasts);

    void Derive(const string& lemma, ConjugationSpec* spec) const;

  private:
    StringTransform pres_part_;
    StringTransform past_part_;
    vector<StringTransform> presents_;
    vector<StringTransform> pasts_;
};

#endif  // CC_PHRASER_ENGLISH_VERB_CONJUGATION_SPEC_DERIVATION_H_
