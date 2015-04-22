#ifndef CC_EN_TAGGING_LAPOS_TOKEN_PREPROCESSOR_H_
#define CC_EN_TAGGING_LAPOS_TOKEN_PREPROCESSOR_H_

#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

class LaposTokenPreprocessor {
  public:
    bool Init();
    void Preprocess(vector<string>* tokens) const;

  private:
    unordered_map<string, string> conv_;
    unordered_map<string, string> unconv_;
};

#endif  // CC_EN_TAGGING_LAPOS_TOKEN_PREPROCESSOR_H_
