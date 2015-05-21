#ifndef CC_PREPROCESS_DESTUTTERER_H_
#define CC_PREPROCESS_DESTUTTERER_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "cc/base/unicode.h"

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
        const ustring& in, size_t max_consecutive, ustring* out,
        vector<uint16_t>* out2in, unordered_map<uchar, size_t>* chr2drop,
        string* error) const;

  private:
    // List of digits in all the scripts of the world.  We don't destutter
    // digits.
    unordered_set<uchar> digits_;
};

#endif  // CC_PREPROCESS_DESTUTTERER_H_
