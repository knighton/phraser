#ifndef CC_PREPROCESS_HTML_ENTITY_REPLACER_H_
#define CC_PREPROCESS_HTML_ENTITY_REPLACER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/misc/unicode.h"

using std::string;
using std::unordered_map;
using std::vector;

class HTMLEntityParser {
  public:
    bool Init(string* error);

    // HTML -> Unicode.
    //
    // Replace HTML entities in Unicode strings with their Unicode character
    // equivalents.
    //
    // Save mapping of 'out' code point -> beginning of span in 'in' code
    // points that it maps to into 'out2in'.
    void Replace(const vector<UChar>& in, vector<UChar>* out,
                 vector<uint16_t>* out2in) const;

  private:
    bool ParseHTMLEntity(
        const vector<UChar>& in, size_t begin, size_t end_excl,
        UChar* code) const;

    void AppendPossibleHTMLEntity(
        const vector<UChar>& in, size_t amp_index, size_t semicolon_index,
        vector<UChar>* out, vector<uint16_t>* out2in) const;

    bool IsPossibleHTMLEntityChar(UChar c) const;

    size_t max_name_length_;
    unordered_map<string, UChar> name2code_;
};

#endif  // CC_PREPROCESS_HTML_ENTITY_REPLACER_H_
