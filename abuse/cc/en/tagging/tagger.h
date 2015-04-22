#ifndef CC_EN_TAGGING_TAGGER_H_
#define CC_EN_TAGGING_TAGGER_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class Tagger {
  public:
    virtual ~Tagger() = 0;

    virtual bool Tag(const vector<string>& tokens, vector<string>* tags) = 0;
};

#endif  // CC_EN_TAGGING_TAGGER_H_
