#ifndef CC_ENGLISH_VERB_VERB_EVALUATOR_H_
#define CC_ENGLISH_VERB_VERB_EVALUATOR_H_

#include "cc/expression/precomputable_evaluator.h"
#include "cc/english/verb/conjugator.h"

class VerbEvaluator : public PrecomputableEvaluator {
  public:
    VerbEvaluator();
    ~VerbEvaluator();

    // Takes ownership.
    bool Init(Conjugator* conj, string* error);

    bool GetExpressionMatches(
            const Expression& expr, vector<string>* tokens) const;

  private:
    bool AreArgsPossible(const vector<string>& args, string* error) const;

    Conjugator* conj_;
};

#endif  // CC_ENGLISH_VERB_VERB_EVALUATOR_H_
