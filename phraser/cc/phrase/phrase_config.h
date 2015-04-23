#ifndef CC_PHRASING_PHRASE_CONFIG_H_
#define CC_PHRASING_PHRASE_CONFIG_H_

#include <string>
#include <vector>

#include "cc/english/english_expression_evaluator.h"

using std::string;
using std::vector;

class PhraseConfig {
  public:
    const string& name() const { return name_; }
    const vector<string>& block_names() const { return block_names_; }
    const vector<vector<vector<TokenGroupID>>>& blocks() const { return blocks_; }

    void Init(const string& name, const vector<string>& block_names,
              const vector<vector<vector<TokenGroupID>>>& blocks);

  private:
    string name_;
    vector<string> block_names_;
    vector<vector<vector<TokenGroupID>>> blocks_;
};

#endif  // CC_PHRASING_PHRASE_CONFIG_H_
