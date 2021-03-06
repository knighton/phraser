#ifndef CC_PHRASE_DETECTION_PHRASE_CONFIG_H_
#define CC_PHRASE_DETECTION_PHRASE_CONFIG_H_

#include <string>
#include <vector>

#include "cc/expression/expression_evaluator.h"
#include "cc/base/json.h"

using std::string;
using std::vector;

struct PhraseConfig {
    string name;
    vector<string> block_names;
    vector<vector<vector<TokenGroupID>>> blocks;

    json::Object* ToJSON() const;
};

#endif  // CC_PHRASE_DETECTION_PHRASE_CONFIG_H_
