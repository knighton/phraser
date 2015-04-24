#ifndef CC_TAGGING_TAGGER_H_
#define CC_TAGGING_TAGGER_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class Tagger {
  public:
    virtual ~Tagger();

    virtual bool Tag(const vector<string>& tokens, vector<string>* tags,
                     string* error) = 0;
};

#endif  // CC_TAGGING_TAGGER_H_
