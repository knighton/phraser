#ifndef CC_BASE_UNICODE_H_
#define CC_BASE_UNICODE_H_

#include <cstdint>
#include <vector>

using std::vector;

typedef uint32_t uchar;
typedef vector<uchar> ustring;

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
     ((uint32_t)(c)<=0xfdef || ((c)&0xfffe)==0xfffe) && \
     (uint32_t)(c)<=0x10ffff)

#endif  // CC_BASE_UNICODE_H_
