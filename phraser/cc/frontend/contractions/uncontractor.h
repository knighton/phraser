#ifndef CC_FRONTEND_CONTRACTIONS_UNCONTRACTOR_H_
#define CC_FRONTEND_CONTRACTIONS_UNCONTRACTOR_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/frontend/base/token.h"
#include "cc/frontend/base/word_and_tag.h"

using std::string;
using std::unordered_map;
using std::vector;

struct BigramNormalization {
    string base;
    string suffix;
    string to_first;
    string to_second;
};

// Having to only uncontract is much simpler than deciding when and how to
// contract.
class Uncontractor {
  public:
    // Set up the tables.
    void Init();

    // Uncontract the tokens.
    //
    // Examples:
    // * tom 's -> tom -POS-
    // * james ' -> james -POS-
    // * its -> it -POS-
    // * wo n't -> will not
    // * she 's -> [she is, she has]
    void Uncontract(const vector<WordAndTag>& in, vector<Token>* out) const;

  private:
    // Always eats the input word.
    void UncontractLoneUnigram(const WordAndTag& wt, vector<Token>* out) const;

    // Returns how many input words it ate (1 or 2).  If zero, it will get sad.
    size_t UncontractBigram(
        const WordAndTag& base, const WordAndTag& suffix,
        vector<Token>* out) const;

    // This list is tiny in practice (won't and let's) and can be simply
    // iterated.
    vector<BigramNormalization> bigram_exceptions_;

    // Contraction suffix -> possible original words.
    unordered_map<string, vector<string>> suffix2uncontracted_;
};

#endif  // CC_FRONTEND_CONTRACTIONS_UNCONTRACTOR_H_
