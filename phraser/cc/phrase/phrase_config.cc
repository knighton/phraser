#include "phrase_config.h"

void PhraseConfig::Init(
        const string& name, const vector<string>& block_names,
        const vector<vector<vector<TokenGroupID>>>& blocks) {
    name_ = name;
    block_names_ = block_names;
    blocks_ = blocks;
}
