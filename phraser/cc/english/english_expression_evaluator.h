#ifndef CC_ENGLISH_ENGLISH_EXPRESSION_EVALUATOR_H_
#define CC_ENGLISH_ENGLISH_EXPRESSION_EVALUATOR_H_

#include "cc/english/english_config.h"
#include "cc/english/personal/personal_manager.h"
#include "cc/expression/expression_evaluator.h"

#include <string>

using std::string;

class EnglishExpressionEvaluator : public ExpressionEvaluator {
  public:
    // Returns false on error.
    bool InitWithConfig(const EnglishConfig& config, string* error);

  private:
    PersonalManager personal_mgr_;
};

#endif  // CC_ENGLISH_ENGLISH_EXPRESSION_EVALUATOR_H_
