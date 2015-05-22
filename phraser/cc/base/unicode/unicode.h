#ifndef CC_BASE_UNICODE_UNICODE_H_
#define CC_BASE_UNICODE_UNICODE_H_

#include <cstddef>
#include <cstdint>
#include <vector>

using std::vector;

// A single Unicode code point.
//
// Note on naming: I (JK) am being deliberately obtuse in calling this a 'ucode'
// and not a 'uchar' because user-perceived characters can be composed of
// multiple code points, and bad things will happen if you assume otherwise.
// You can't use them like ASCII chars.
typedef uint32_t ucode;

/**
 * From common/uicode/utf.h:
 *
 * Is this code point a Unicode noncharacter?
 * @param c 32-bit code point
 * @return TRUE or FALSE
 * @stable ICU 2.4
 */
#define U_IS_UNICODE_NONCHAR(c) \
    ((c)>=0xfdd0 && \
     ((ucode)(c)<=0xfdef || ((c)&0xfffe)==0xfffe) && \
     (ucode)(c)<=0x10ffff)

// A unicode code point's canonical combining class (see
// http://www.unicode.org/versions/Unicode7.0.0/ch03.pdf#G49537).
typedef uint8_t UnicodeCombiningClass;

// A string containing Unicode code points.
typedef vector<ucode> ustring;

// Dump the Unicode string's hex values to stdout.
void DumpUString(const ustring& s);

// A user-perceived Unicode character.
//
// Contains indexes into a ustring.  Does not own memory.
struct UnicodeSpan {
    size_t begin;
    size_t end_excl;
};

#endif  // CC_BASE_UNICODE_UNICODE_H_
