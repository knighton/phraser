#ifndef CC_PHRASING_PHRASE_CONFIG_H_
#define CC_PHRASING_PHRASE_CONFIG_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

struct PhraseConfig {
    string name;
    vector<string> block_names;
    vector<vector<vector<TokenGroupID>>> blocks;
};

#endif  // CC_PHRASING_PHRASE_CONFIG_H_
