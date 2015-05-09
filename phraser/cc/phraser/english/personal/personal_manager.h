#ifndef CC_PHRASER_ENGLISH_PERSONAL_PERSONAL_MANAGER_H_
#define CC_PHRASER_ENGLISH_PERSONAL_PERSONAL_MANAGER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/phraser/expression/expression.h"

using std::string;
using std::unordered_map;
using std::vector;

struct PersonalTokenInfo {
    string token;
    unordered_map<string, string> dimension2value;
};

// Holds information about "personals" (personal pronouns, possessive pronouns,
// and personal determiners).
class PersonalManager {
  public:
    // Returns false on error.
    bool Init(string* error);

    // The three types of personal expression evaluators (they all call into
    // here) are:
    // * PersProEvaluator
    // * PosProEvaluator
    // * PosDetEvaluator
    //
    // They are all precomputed evaluator types.  Therefore, this method is only
    // called during Init() to precompute token matches.  Therefore, we don't
    // need to be blazing fast.
    //
    // Returns false on error (invalid Expression type).
    bool GetExpressionMatches(
        const Expression& expr, vector<string>* tokens) const;

  private:
    // List of tokens and information about them.
    //
    // Token string values are not unique (ie, words can mean different things).
    //
    // Lookup doesn't need to be particularly efficient (see above).
    vector<PersonalTokenInfo> pers_pro_infos_;
    vector<PersonalTokenInfo> pos_pro_infos_;
    vector<PersonalTokenInfo> pos_det_infos_;

    // Expression type -> one of the three vectors above.
    unordered_map<string, vector<PersonalTokenInfo>*> type2tokeninfos_;
};

#endif  // CC_PHRASER_ENGLISH_PERSONAL_PERSONAL_MANAGER_H_
