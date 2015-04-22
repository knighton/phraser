#ifndef CC_EN_SPECIAL_TOKENS_VERB_VERB_EVALUATOR_H_
#define CC_EN_SPECIAL_TOKENS_VERB_VERB_EVALUATOR_H_

#include "cc/base/token_categorization/precomputable_evaluator.h"
#include "cc/en/special_tokens/verb/conjugator.h"

class VerbEvaluator : public PrecomputableEvaluator {
  public:
    VerbEvaluator();
    ~VerbEvaluator();

    // Takes ownership.
    void Init(Conjugator* conj);

    bool GetExpressionMatches(
            const Expression& expr, vector<string>* tokens) const;

  private:
    bool AreArgsPossible(const vector<string>& args) const;

    Conjugator* conj_;
};

#endif  // CC_EN_SPECIAL_TOKENS_VERB_VERB_EVALUATOR_H_
