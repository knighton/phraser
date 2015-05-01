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
    // Processes the text in |dirty| and puts it in |clean|, optionally saving a
    // mapping to the original indexes in |clean2dirty_or_null| and/or a mapping
    // of code point to drop count in |chr2drop_or_null|.
    //
    // All non-digit code points past |max_consecutive| will be dropped.  If set
    // to 0, destuttering will not be applied.
    //
    // Does not perform any normalization before destuttering.  That is up to
    // the caller.
    //
    // Operates on Unicode code points, not "characters" (so no special
    // treatment of combining characters, etc.).
    //
    // Returns false on failure.  If it failed, saves reason in |error|.
    bool Destutter(
        const vector<UChar>& dirty, size_t max_consecutive,
        vector<UChar>* clean, vector<uint16_t>* clean2dirty_or_null,
        unordered_map<UChar, size_t>* chr2drop_or_null, string* error) const;

  private:
    // List of digits in all the scripts of the world.  We don't destutter
    // digits.
    unordered_set<UChar> digits_;
};

#endif  // CC_PREPROCESS_DESTUTTERER_H_
