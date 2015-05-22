#ifndef CC_PREPROCESS_PREPROCESSOR_H_
#define CC_PREPROCESS_PREPROCESSOR_H_

#include <string>
#include <vector>
#include <unordered_set>

#include "cc/base/unicode/unicode.h"
#include "cc/preprocess/destutterer.h"
#include "cc/preprocess/html_entity_parser.h"

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
        const ustring& original, bool replace_html_entities,
        size_t destutter_max_consecutive, ustring* clean,
        vector<uint16_t>* clean2original_or_null,
        unordered_map<ucode, size_t>* chr2drop_or_null, string* error) const;

  private:
    bool ready_;
    HTMLEntityParser html_;
    Destutterer destutter_;
};

#endif  // CC_PREPROCESS_PREPROCESSOR_H_
