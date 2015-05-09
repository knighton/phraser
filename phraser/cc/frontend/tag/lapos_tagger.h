#ifndef CC_FRONTEND_TAG_LAPOS_TAGGER_H_
#define CC_FRONTEND_TAG_LAPOS_TAGGER_H_

#include <string>
#include <vector>

#include "cc/third_party/lapos/crf.h"

using std::string;
using std::vector;

class LaposTagger {
  public:
    LaposTagger();
    ~LaposTagger();

    bool Init();

    bool Tag(const vector<string>& tokens, vector<string>* tags, string* error);

  private:
    bool is_ok_;
    lapos::CRF_Model crfm_;
};

#endif  // CC_FRONTEND_TAG_LAPOS_TAGGER_H_
