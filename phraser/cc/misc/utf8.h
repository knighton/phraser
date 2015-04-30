#ifndef CC_MISC_UTF8_H_
#define CC_MISC_UTF8_H_

#include <string>

#include "cc/misc/unicode.h"

using std::string;

namespace utf8 {

// Append the code point's UTF-8 representation to the string.
void Append(UChar c, string* s);

// Read the next code point from a UTF-8 encoded string.
//
// Returns false when (a) out of string or (b) invalid data.
bool Read(const string& s, size_t* x, UChar* c);

// Read the next UTF-8 code point.  Returns false if it is not the expected
// character or the read returned false.
bool Expect(const string& s, size_t* x, UChar* c, UChar expected);

}  // namespace unicode

#endif  // CC_MISC_UTF8_8_
