#ifndef CC_PHRASER_ENGLISH_PERSONAL_POS_PRO_EVALUATOR_H_
#define CC_PHRASER_ENGLISH_PERSONAL_POS_PRO_EVALUATOR_H_

#include "cc/phraser/english/personal/personal_manager.h"
#include "cc/phraser/expression/precomputable_evaluator.h"

class PosProEvaluator : public PrecomputableEvaluator {
  public:
    // Does not take ownership.
    bool Init(PersonalManager* personal_mgr, string* error);

    bool GetExpressionMatches(
            const Expression& expr, vector<string>* tokens) const;

  private:
    bool AreArgsPossible(const vector<string>& args, string* error) const;

    PersonalManager* personal_mgr_;
};

#endif  // CC_PHRASER_ENGLISH_PERSONAL_POS_PRO_EVALUATOR_H_
