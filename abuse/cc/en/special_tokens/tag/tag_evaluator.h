#ifndef CC_EN_SPECIAL_TOKENS_TAG_TAG_EVALUATOR_H_
#define CC_EN_SPECIAL_TOKENS_TAG_TAG_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/base/token_categorization/all_input_evaluator.h"
#include "cc/en/tagging/tagger.h"

using std::string;
using std::vector;

class TagEvaluator : public AllInputEvaluator<string> {
  public:
    TagEvaluator();
    ~TagEvaluator();

    // Takes ownership.
    void Init(Tagger* tagger);

    bool AnalyzeTokens(
        const vector<string>& tokens, vector<string>* tags) const;

    bool IsMatch(
        const Expression& expr, const string& token,
        const string& tag) const;


  private:
    bool AreArgsPossible(const vector<string>& args) const;

    Tagger* tagger_;
};

#endif  // CC_EN_SPECIAL_TOKENS_TAG_TAG_EVALUATOR_H_
