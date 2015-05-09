#ifndef CC_PHRASER_ENGLISH_NUMBER_NUMBER_EVALUATOR_H_
#define CC_PHRASER_ENGLISH_NUMBER_NUMBER_EVALUATOR_H_

#include <boost/regex.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "cc/expression/dynamic_evaluator.h"

using boost::regex;
using std::string;
using std::unordered_map;
using std::vector;

class NumberEvaluator : public DynamicEvaluator {
  public:
    bool Init(string* error);

    bool MightMatch(const string& token) const;

    void FeaturesFromToken(
        const string& token,
        unordered_map<string, string>* dimension2value) const;

  private:
    bool AreArgsPossible(const vector<string>& args, string* error) const;

    regex might_match_re_;
};

#endif  // CC_PHRASER_ENGLISH_NUMBER_NUMBER_EVALUATOR_H_
