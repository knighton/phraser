#ifndef CC_PHRASER_ENGLISH_PERSONAL_POS_DET_EVALUATOR_H_
#define CC_PHRASER_ENGLISH_PERSONAL_POS_DET_EVALUATOR_H_

#include "cc/expression/precomputable_evaluator.h"
#include "cc/english/personal/personal_manager.h"

class PosDetEvaluator : public PrecomputableEvaluator {
  public:
    // Does not take ownership.
    bool Init(PersonalManager* personal_mgr, string* error);

    bool GetExpressionMatches(
            const Expression& expr, vector<string>* tokens) const;

  private:
    bool AreArgsPossible(const vector<string>& args, string* error) const;

    PersonalManager* personal_mgr_;
};

#endif  // CC_PHRASER_ENGLISH_PERSONAL_POS_DET_EVALUATOR_H_
