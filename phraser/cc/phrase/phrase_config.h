#ifndef CC_PHRASING_PHRASE_CONFIG_H_
#define CC_PHRASING_PHRASE_CONFIG_H_

#include <string>
#include <vector>

#include "cc/expression/expression_evaluator.h"
#include "cc/misc/json.h"

using std::string;
using std::vector;

struct PhraseConfig {
    string name;
    vector<string> block_names;
    vector<vector<vector<TokenGroupID>>> blocks;

    json::Object* ToJSON() const;
};

#endif  // CC_PHRASING_PHRASE_CONFIG_H_
