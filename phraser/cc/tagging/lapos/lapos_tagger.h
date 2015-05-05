#ifndef CC_TAGGING_LAPOS_LAPOS_TAGGER_H_
#define CC_TAGGING_LAPOS_LAPOS_TAGGER_H_

#include <string>
#include <vector>

#include "cc/tagging/lapos/lapos_token_preprocessor.h"
#include "cc/tagging/tagger.h"
#include "cc/third_party/lapos/crf.h"

using std::string;
using std::vector;

class LaposTagger : public Tagger {
  public:
    LaposTagger();
    ~LaposTagger();

    bool Init();

    bool Tag(const vector<string>& tokens, vector<string>* tags, string* error);

  private:
    bool is_ok_;
    LaposTokenPreprocessor preproc_;
    lapos::CRF_Model crfm_;
};

#endif  // CC_TAGGING_LAPOS_LAPOS_TAGGER_H_
