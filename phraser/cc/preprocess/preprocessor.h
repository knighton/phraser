#ifndef CC_PREPROCESS_PREPROCESSOR_H_
#define CC_PREPROCESS_PREPROCESSOR_H_

#include <string>
#include <vector>
#include <unordered_set>

#include "cc/misc/unicode.h"
#include "cc/preprocess/destutterer.h"

using std::string;
using std::vector;
using std::unordered_set;

class Preprocessor {
  public:
    Preprocessor() : ready_(false) {}

    bool Init(string* error);

    bool Preprocess(const vector<UChar>& dirty,
                    size_t destutter_max_consecutive,
                    vector<UChar>* clean, string* error) const;

  private:
    bool ready_;
    Destutterer destutter_;
};

#endif  // CC_PREPROCESS_PREPROCESSOR_H_
