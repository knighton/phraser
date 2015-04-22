#ifndef CC_EN_SPECIAL_TOKENS_TAG_TAG_EVALUATOR_H_
#define CC_EN_SPECIAL_TOKENS_TAG_TAG_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/base/token_categorization/all_token_evaluator.h"
#include "cc/en/tagging/tagger.h"

using std::string;
using std::vector;

class TagEvaluator : public AllTokenEvaluator<string> {
  public:
    TextEvaluator();
    ~TextEvaluator();

    // Takes ownership.
    void Init(Tagger* tagger);

    bool AnalyzeTokens(
        const vector<string>& tokens, vector<string>* tags) const;

    bool IsMatch(
        const Expression& expr, const string& token,
        const string& tag) const;

  private:
    Tagger* tagger;
};

#endif  // CC_EN_SPECIAL_TOKENS_TAG_TAG_EVALUATOR_H_
