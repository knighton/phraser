#ifndef CC_FRONTEND_HTML_HTML_ENTITY_PARSER_H_
#define CC_FRONTEND_HTML_HTML_ENTITY_PARSER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/base/unicode.h"

using std::string;
using std::unordered_map;
using std::vector;

class HtmlEntityParser {
  public:
    bool Init(string* error);

    // HTML -> Unicode.
    //
    // Replace HTML entities in Unicode strings with their Unicode character
    // equivalents.
    //
    // Save mapping of 'out' code point -> beginning of span in 'in' code
    // points that it maps to into 'out2in'.
    //
    // Returns whether it did any replacing.  This is so that it can be called
    // multiple times until done.
    bool Replace(const ustring& in, ustring* out,
                 vector<uint16_t>* out2in) const;

  private:
    bool ParseHtmlEntity(
        const ustring& in, size_t begin, size_t end_excl, uchar* code) const;

    // Returns whether it parsed an HTML entity.
    bool AppendPossibleHtmlEntity(
        const ustring& in, size_t amp_index, size_t semicolon_index,
        ustring* out, vector<uint16_t>* out2in) const;

    bool IsPossibleHtmlEntityChar(uchar c) const;

    size_t max_name_length_;
    unordered_map<string, uchar> name2code_;
};

#endif  // CC_FRONTEND_HTML_HTML_ENTITY_PARSER_H_
