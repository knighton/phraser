#ifndef CC_PREPROCESS_HTML_ENTITY_PARSER_H_
#define CC_PREPROCESS_HTML_ENTITY_PARSER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/base/unicode.h"

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
    void Replace(const ustring& in, ustring* out,
                 vector<uint16_t>* out2in) const;

  private:
    bool ParseHTMLEntity(
        const ustring& in, size_t begin, size_t end_excl, uchar* code) const;

    void AppendPossibleHTMLEntity(
        const ustring& in, size_t amp_index, size_t semicolon_index,
        ustring* out, vector<uint16_t>* out2in) const;

    bool IsPossibleHTMLEntityChar(uchar c) const;

    size_t max_name_length_;
    unordered_map<string, uchar> name2code_;
};

#endif  // CC_PREPROCESS_HTML_ENTITY_PARSER_H_
