#ifndef CC_PHRASER_ENGLISH_ENGLISH_EXPRESSION_EVALUATOR_H_
#define CC_PHRASER_ENGLISH_ENGLISH_EXPRESSION_EVALUATOR_H_

#include "cc/phraser/english/personal/personal_manager.h"
#include "cc/phraser/expression/expression_evaluator.h"

#include <string>

using std::string;

class EnglishExpressionEvaluator : public ExpressionEvaluator {
  public:
    // Returns false on fatal error.
    bool Init(string* error);

  private:
    PersonalManager personal_mgr_;
};

#endif  // CC_PHRASER_ENGLISH_ENGLISH_EXPRESSION_EVALUATOR_H_
