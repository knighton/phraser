#ifndef CC_EN_SPECIAL_TOKENS_NUMBER_EVALUATOR_H_
#define CC_EN_SPECIAL_TOKENS_NUMBER_EVALUATOR_H_

#include <boost/regex.hpp>

#include "cc/base/token_categorization/dynamic_evaluator.h"

using boost::regex;

class NumberEvaluator : public DynamicEvaluator {
  public:
    void Init();

    bool MightMatch(const string& token) const;

    void FeaturesFromToken(
        const string& token,
        unordered_map<string, string>* dimension2value) const;

  private:
    bool AreArgsPossible(const vector<string>& args) const;

    regex might_match_re_;
};

#endif  // CC_EN_SPECIAL_TOKENS_NUMBER_EVALUATOR_H_
