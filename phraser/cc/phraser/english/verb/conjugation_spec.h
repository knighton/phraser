#ifndef CC_PHRASER_ENGLISH_VERB_CONJUGATION_SPEC_H_
#define CC_PHRASER_ENGLISH_VERB_CONJUGATION_SPEC_H_

#include <string>
#include <vector>

#include "cc/misc/enum_strings.h"
#include "cc/expression/token_with_features.h"

using std::string;
using std::vector;

enum ConjSpecFieldType {
    CSFT_LEMMA,
    CSFT_PRES_PART,
    CSFT_PAST_PART,
    CSFT_FINITE_PRES,
    CSFT_FINITE_PAST,
    NUM_CONJ_SPEC_FIELD_TYPES,
};

extern EnumStrings<ConjSpecFieldType> ConjSpecFieldTypeStrings;

class ConjugationSpec {
  public:
    const string& lemma() const { return lemma_; }
    const string& pres_part() const { return pres_part_; }
    const string& past_part() const { return past_part_; }
    const vector<string>& pres() const { return pres_; }
    const vector<string>& past() const { return past_; }

    void Init(const string& lemma, const string& pres_part,
              const string& past_part, const vector<string>& pres,
              const vector<string>& past);

    const string& GetField(size_t field_index) const;

    // Returns false on internal error.
    bool EachTokenWithFeatures(vector<TokenWithFeatures>* out) const;

  private:
    string lemma_;
    string pres_part_;
    string past_part_;
    vector<string> pres_;
    vector<string> past_;
};

#endif  // CC_PHRASER_ENGLISH_VERB_CONJUGATION_SPEC_H_
