#ifndef CC_EN_TAGGING_LAPOS_TAGGER_H_
#define CC_EN_TAGGING_LAPOS_TAGGER_H_

#include <string>
#include <vector>

#include "cc/en/tagging/lapos_token_preprocessor.h"
#include "cc/en/tagging/tagger.h"
#include "cc/third_party/lapos/crf.h"

using std::string;
using std::vector;

class LaposTagger : public Tagger {
  public:
    LaposTagger();
    bool Init(const string& model_f);
    bool Tag(const vector<string>& tokens, vector<string>* tags);

  private:
    bool is_ok_;
    LaposTokenPreprocessor preproc_;
    lapos::CRF_Model crfm_;
};

#endif  // CC_EN_TAGGING_LAPOS_TAGGER_H_
