#ifndef CC_ENGLISH_ENGLISH_CONFIG_H_
#define CC_ENGLISH_ENGLISH_CONFIG_H_

#include <string>

using std::string;

// Configuration needed to initialize an EnglishExpressionEvaluator
// (specifically, its various Expression type evaluators).
struct EnglishConfig {
    string lapos_model_f;
};

#endif  // CC_ENGLISH_ENGLISH_CONFIG_H_
