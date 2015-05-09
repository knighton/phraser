#ifndef CC_PHRASER_ENGLISH_VERB_CONJUGATOR_H_
#define CC_PHRASER_ENGLISH_VERB_CONJUGATOR_H_

#include <string>
#include <unordered_map>

#include "cc/phraser/english/verb/conjugation_spec.h"
#include "cc/phraser/english/verb/conjugation_spec_derivation.h"

using std::string;
using std::unordered_map;

class Conjugator {
  public:
    void Init(const unordered_map<string, ConjugationSpec>& lemma2irregular,
              const ConjugationSpecDerivation& regular_deriv);

    // Returns false if the lemma is not entirely lowercase alpha.
    void Conjugate(const string& lemma, ConjugationSpec* spec) const;

  private:
    unordered_map<string, ConjugationSpec> lemma2irregular_;
    ConjugationSpecDerivation regular_deriv_;
};

#endif  // CC_PHRASER_ENGLISH_VERB_CONJUGATOR_H_
