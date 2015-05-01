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

    // Returns false on failure.
    bool Init(string* error);

    // Dirty -> clean, with some optional additional info.
    //
    // Returns false on failure.
    bool Preprocess(
        const vector<UChar>& dirty, size_t destutter_max_consecutive,
        vector<UChar>* clean, vector<uint16_t>* clean2dirty_or_null,
        unordered_map<UChar, size_t>* chr2drop_or_null, string* error) const;

  private:
    bool ready_;
    Destutterer destutter_;
};

#endif  // CC_PREPROCESS_PREPROCESSOR_H_
