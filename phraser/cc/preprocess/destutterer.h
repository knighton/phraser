#ifndef CC_PREPROCESS_DESTUTTERER_H_
#define CC_PREPROCESS_DESTUTTERER_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "cc/misc/unicode.h"

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

class Destutterer {
  public:
    // Returns false on error.
    bool Init(string* error);

    // Normalize text by removing excessive duplicate characters.
    //
    //     "happppy" -> "happy"
    //
    // 'max_consecutive' sets the maximum allowed number of consecutive
    // identical code points.  All code points over the limit will be dropped,
    // with the except of digit characters.  If 'max_consecutive' is 0,
    // destuttering will not be applied.
    //
    // chr2drop can be NULL.  If NULL, that data will not be collected.
    //
    // Does not perform any Unicode normalization before destuttering.
    //
    // Returns false on failure.
    bool Destutter(const vector<UChar>& dirty, size_t max_consecutive,
                   vector<UChar>* clean, unordered_map<UChar, size_t>* chr2drop,
                   string* error) const;

  private:
    // List of digits in all the scripts of the world.  We don't destutter
    // digits.
    unordered_set<UChar> digits_;
};

#endif  // CC_PREPROCESS_DESTUTTERER_H_
