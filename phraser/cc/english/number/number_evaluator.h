#ifndef CC_ENGLISH_NUMBER_NUMBER_EVALUATOR_H_
#define CC_ENGLISH_NUMBER_NUMBER_EVALUATOR_H_

#include <boost/regex.hpp>

#include "cc/expression/dynamic_evaluator.h"

using boost::regex;

class NumberEvaluator : public DynamicEvaluator {
  public:
    void Init();

    bool MightMatch(const string& token) const;

    void FeaturesFromToken(
        const string& token,
        unordered_map<string, string>* dimension2value) const;

  private:
    bool AreArgsPossible(const vector<string>& args, string* error) const;

    regex might_match_re_;
};

#endif  // CC_ENGLISH_NUMBER_NUMBER_EVALUATOR_H_
